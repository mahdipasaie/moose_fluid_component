import fenics as fe
import numpy as np
from tqdm import tqdm 

file = fe.XDMFFile("ns2.xdmf") 
file.parameters["rewrite_function_mesh"] = True
file.parameters["flush_output"] = True
file.parameters["functions_share_mesh"] = True

params ={
    "dy": 0.01,
    "dt": 1,
    "xmax": 1,
    "ymax": 0.5,
    "mu": 1, 
    "rho":1,
    "velx":1,
}
solver_parameters ={
'nonlinear_solver': "snes" ,
'snes_solver': {
'linear_solver': "mumps" ,
'report': False ,
"preconditioner":"hypre_amg" ,
'error_on_nonconvergence': False ,
'absolute_tolerance': 1E-6 ,
'relative_tolerance': 1E-5 ,
'maximum_iterations': 50 ,}
}
params["solver_parameters"] = solver_parameters
######## mesh
nx= (int)(params['xmax']/params["dy"])
ny= (int)(params["ymax"]/params["dy"])
mesh = fe.RectangleMesh(fe.Point(0, 0), fe.Point(params["xmax"], params["ymax"]), nx, ny)
########

def epsilon(u):  
    return 0.5*(fe.grad(u)+fe.grad(u).T)

def sigma(u, p):
    return 2*epsilon(u) - p*fe.Identity(len(u))

def func_space(mesh):

    P1 = fe.FiniteElement("Lagrange", mesh.ufl_cell(), 2)  
    P11 = fe.FiniteElement("Lagrange", mesh.ufl_cell(), 2)  
    P2 = fe.FiniteElement("Lagrange", mesh.ufl_cell(), 1) 
    EL = fe.MixedElement([P1, P11, P2])
    fs = fe.FunctionSpace(mesh, EL)
    tx, ty, tq = fe.TestFunctions(fs)
    sv = fe.Function(fs)  
    sv_ = fe.Function(fs)  
    ux, uy, p = fe.split(sv)  
    ux_, uy_, p_ = fe.split(sv_) 

    return { "ux":ux, "ux_":ux_, "uy":uy, "uy_":uy_, "p":p, "p_":p_, 
    "tx":tx, "ty":ty, "tq":tq,
    "fs":fs, "sv":sv, "sv_":sv_,
    }

def form(var, params):

    # var
    ux = var["ux"]
    uy = var["uy"]
    ux_ = var["ux_"]
    uy_ = var["uy_"]
    p = var["p"]
    tx = var["tx"]
    ty = var["ty"]
    tq = var["tq"]
    # params
    dt = params["dt"]
    mu = params["mu"]
    rho = params["rho"]

    F1x = (
        fe.inner( fe.inner(fe.as_vector((ux, uy)), fe.grad(ux)), tx) + 
        fe.inner(-p, tx.dx(0)) +
        (mu/rho)* fe.inner(fe.grad(ux), fe.grad(tx)) +
        fe.inner((ux-ux_)/dt, tx)
    )
    F1y = (
        fe.inner( fe.inner(fe.as_vector((ux, uy)), fe.grad(uy)), ty) + 
        fe.inner(-p, ty.dx(1)) +
        (mu/rho)* fe.inner(fe.grad(uy), fe.grad(ty)) +
        fe.inner((uy-uy_)/dt, ty)
    )
    Fp = (
        fe.inner(fe.div(fe.as_vector((ux, uy))), tq)
    )
    F = (F1x+ F1y+ Fp)*fe.dx

    return F


# def BC(var, params):
#     # var
#     fs = var["fs"]
#     # par
#     velx = params["velx"]
#     xmax = params.get("xmax")
#     ymax = params.get("ymax")
#     inflow = 'near(x[0],0)'
#     outflow = f'near(x[0],{xmax})'
#     walls = f'near(x[1],0) || near(x[1],{ymax})'
#     bcp_outflow = fe.DirichletBC(fs.sub(1), fe.Constant(0), outflow)
#     bc_walls = fe.DirichletBC(fs.sub(0), fe.Constant((0.0, 0.0)), walls)
#     bcu_inflow = fe.DirichletBC(fs.sub(0), fe.Constant((velx, 0.0)), inflow)
#     Bc = [bcu_inflow, bcp_outflow, bc_walls]
#     return Bc

def BC(var, params):
    # Variables
    fs = var["fs"]
    
    # Parameters
    velx = params["velx"]  # Horizontal velocity of the lid
    xmax = params.get("xmax")  # Width of the cavity, defaulting to 1.0
    ymax = params.get("ymax")  # Height of the cavity, defaulting to 1.0
    
    # Boundary definitions
    inflow = f'near(x[1], {ymax})'  # Lid (moving at the top)
    walls = f'near(x[1],0) || near(x[0],0) || near(x[0],{xmax})'  # No-slip walls (left, right, and bottom)
    
    # Boundary conditions
    bcu_lid_x = fe.DirichletBC(fs.sub(0), fe.Constant(velx), inflow)  # Moving lid
    bcu_lid_y = fe.DirichletBC(fs.sub(1), fe.Constant(0.0), inflow)  # Moving lid
    bc_walls_x = fe.DirichletBC(fs.sub(0), fe.Constant(0.0), walls)  # No-slip walls
    bc_walls_y = fe.DirichletBC(fs.sub(1), fe.Constant(0.0), walls)  # No-slip walls

    
    # Return boundary conditions as a list
    Bc = [bcu_lid_x, bcu_lid_y, bc_walls_x, bc_walls_y]
    return Bc

def solver(F, Bc, var, params):
    # var
    sv = var["sv"]
    solver_parameters = params["solver_parameters"]
    J= fe.derivative(F, sv)
    problem= fe.NonlinearVariationalProblem(F, sv, Bc, J)
    solver = fe.NonlinearVariationalSolver(problem)
    solver.parameters.update(solver_parameters)
    return solver

def InitialC(var):

    sv = var["sv"]
    sv_ = var["sv_"]

    class InitialConditions_ns(fe.UserExpression):

        def __init__(self, **kwargs):
            super().__init__(**kwargs)  

        def eval(self, values, x):
            values[0] = 0.0  
            values[1] = 0.0  
            values[2] = 0.0  

        def value_shape(self):
            return (3,)


    sv_.interpolate(InitialConditions_ns(degree=2))
    sv.interpolate(InitialConditions_ns(degree=2))

    return

#################
var = func_space(mesh)
F = form(var, params)
Bc = BC(var, params)
InitialC(var)
solver = solver(F, Bc, var, params)
sv = var["sv"]
sv_ = var["sv_"]
#################
dt = params["dt"]
T = 0
for it in tqdm(range(1000)):

    if it%10== 0 :
        ux_, uy_, p_ = sv_.split(deepcopy=True)
        ux_.rename("ux", "ux")  
        uy_.rename("uy", "uy")  
        p_.rename("p", "p")
        file.write(ux_ , T)
        file.write(uy_ , T)
        file.write(p_ , T)
        file.close()

    T += dt
    solver.solve()
    sv_.vector()[:]= sv.vector()






