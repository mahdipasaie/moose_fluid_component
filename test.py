import fenics as fe
import numpy as np
from tqdm import tqdm 

file = fe.XDMFFile("ns.xdmf") 
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

    P1 = fe.VectorElement("Lagrange", mesh.ufl_cell(), 2)  
    P2 = fe.FiniteElement("Lagrange", mesh.ufl_cell(), 1) 
    EL = fe.MixedElement([P1, P2])
    fs = fe.FunctionSpace(mesh, EL)
    v, q = fe.TestFunctions(fs)
    sv = fe.Function(fs)  
    sv_ = fe.Function(fs)  
    u, p = fe.split(sv)  
    u_, p_ = fe.split(sv_) 
    space_u, _ = fs.sub(0).collapse(collapsed_dofs=True)
    space_p, _ = fs.sub(1).collapse(collapsed_dofs=True)

    return { "u":u, "u_":u_, 
    "q":q, "v":v, "p":p,
    "fs":fs, "sv":sv, "sv_":sv_,
    }

def form(var, params):

    # var
    u = var["u"]
    u_ = var["u_"]
    q = var["q"]
    v = var["v"]
    p = var["p"]
    # params
    dt = params["dt"]
    mu = params["mu"]
    rho = params["rho"]

    Fp = fe.inner(fe.div(u)/dt, q)  
    Fu = (
        fe.inner((u - u_)/dt, v) 
        + fe.inner(fe.dot(u,fe.grad(u)),v) 
        + mu/rho*fe.inner(sigma(u, p), epsilon(v)) 
    )
    F = (Fp + Fu)*fe.dx
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
    bcu_lid = fe.DirichletBC(fs.sub(0), fe.Constant((velx, 0.0)), inflow)  # Moving lid
    bc_walls = fe.DirichletBC(fs.sub(0), fe.Constant((0.0, 0.0)), walls)  # No-slip walls
    
    # Return boundary conditions as a list
    Bc = [bcu_lid, bc_walls]
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

    T += dt
    solver.solve()
    sv_.vector()[:]= sv.vector()
    if it%10== 0 :
        u_, p_ = sv_.split(deepcopy=True)
        u_.rename("u", "u")  
        p_.rename("p", "p")
        file.write(u_ , T)
        file.write(p_ , T)
        file.close()





