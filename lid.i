[Mesh]
    [gen]
      type = GeneratedMeshGenerator
      dim = 2
      nx = 100
      ny = 100
      xmin = 0.0
      xmax = 1.0
      ymin = 0.0
      ymax = 0.5
      elem_type = QUAD9
    [../]
    [./corner_node]
      type = ExtraNodesetGenerator
      new_boundary = 'pinned_node'
      nodes = '0'
      input = gen
    [../]
[]
  
[Variables]
    [./ux]
        family = LAGRANGE
        order = SECOND
    [../]
    [./uy]
        family = LAGRANGE
        order = SECOND
    [../]
    [./p]
        family = LAGRANGE
        order = FIRST
    [../]
    # [./lambda]
    #     family = SCALAR
    #     order = FIRST
    # [../]
[]

[Materials]
    [./rho]
      type = ADGenericConstantMaterial
      prop_names = 'rho'
      prop_values = '1' 
    [../]
    [./mu]
        type = ADGenericConstantMaterial
        prop_names = 'mu'
        prop_values = '1' 
    [../]
[]

[Kernels]
    [./T_x] #  x-component
        type = CoefTimeDerivative
        variable = ux
        Coefficient = 1
    [../]
    [./T_y] # y-component
        type = CoefTimeDerivative
        variable = uy
        Coefficient = 1
    [../]
    [./vis_x] # < -p * I + mu * grad(u), grad(test) > x-component
        type = f1
        variable = ux
        p = p
        rho = rho
        mu = mu 
    [../]
    [./vis_y] # < -p * I + mu * grad(u), grad(test) > y-component
        type = f2
        variable = uy
        p = p
        rho = rho
        mu = mu 
    [../]
    [./adv_x] # ρ(u·∇)u x-component
        type = f3
        variable = ux
        uy = uy
    [../]
    [./adv_y] # ρ(u·∇)u y-component
        type = f4
        variable = uy
        ux = ux
    [../]
    [./cont] # ∇·u = 0
        type = f5
        variable = p
        ux = ux
        uy = uy
    [../] 
    [./p_x] 
        type = f6
        variable = ux
        p = p
    [../] 
    [./p_y] 
        type = f7
        variable = uy
        p = p
    [../] 

    # [./mean_zero_pressure]
    #     type = ScalarLagrangeMultiplier
    #     variable = p
    #     lambda = lambda
    # [../]
[]


# [ScalarKernels]
#   [./mean_zero_pressure_lm]
#     type = AverageValueConstraint
#     variable = lambda
#     pp_name = pressure_integral
#     value = 0
#   [../]
# []

[BCs]
    # [./no_slip_left]
    #     type = DirichletBC
    #     boundary = 'left'
    #     variable = ux
    #     value = 0.1
    # [../]
    [./no_slip_left]
        type = FunctionDirichletBC
        boundary = 'left'
        function = inlet_func
        variable = ux
    [../]
    [./no_slip_left_y]
        type = DirichletBC
        boundary = 'left'
        variable = uy
        value = 0.0
    [../]
    # [./no_slip_right]
    #     type = DirichletBC
    #     boundary = 'right'
    #     variable = ux
    #     value = 0.0
    # [../]
    [./no_slip_right]
        type = DirichletBC
        boundary = 'right'
        variable = p
        value = 0.0
    [../]
    [./no_slip_bottom]
        type = DirichletBC
        boundary = 'bottom'
        variable = ux
        value = 0.0
    [../]
    [./no_slip_bottom_y]
        type = DirichletBC
        boundary = 'bottom'
        variable = uy
        value = 0.0
    [../]
    # [./no_slip_right_y]
    #     type = DirichletBC
    #     boundary = 'right'
    #     variable = uy
    #     value = 0.0
    # [../]
    [./moving_lid_x]
        type = DirichletBC
        boundary = 'top'
        variable = ux
        value = 0
    [../]
    [./moving_lid_y]
        type = DirichletBC
        boundary = 'top'
        variable = uy
        value = 0.0
    [../]
[]

[ICs]
    [ux_ic]
      type = FunctionIC
      variable = 'ux'
      function = zerofunc
    []
    [uy_ic]
        type = FunctionIC
        variable = 'uy'
        function = zerofunc
    []
    [p_ic]
        type = FunctionIC
        variable = 'p'
        function = zerofunc
    []
[]


[Preconditioning]
    [./advanced_smp_precond]
        type = SMP
        full = true
    [../]
[]

[Functions]
    [./inlet_func]
        type = ParsedFunction
        value = ' sqrt((x-2)^2 * (x+2)^2 * (y-2)^2 * (y+2)^2) / 16'
    [../]
    [zerofunc]
        type = ParsedFunction
        value = '0'
    [../]
[]
# [./moving_lid_x]
#     type = FunctionDirichletBC
#     function = inlet_func 
#     variable = ux
#     boundary = inlet 
# [../]    

# [Postprocessors]
#     [./pressure_integral]
#       type = ElementIntegralVariablePostprocessor
#       variable = p
#       execute_on = linear
#     [../]
#   []

[Executioner]
    type = Transient
    solve_type = PJFNK                   
    nl_abs_tol = 1e-8                     
    nl_rel_tol = 1e-8                     
    l_max_its = 200                        
    nl_max_its = 60                        
    # petsc_options_iname = '-ksp_type -pc_type -pc_hypre_type -ksp_gmres_restart -pc_hypre_boomeramg_strong_threshold'
    # petsc_options_value = 'gmres hypre boomeramg 30 0.7'
    petsc_options = ' -snes_converged_reason -ksp_converged_reason '
    petsc_options_iname = ' -pc_type -pc_factor_shift_type -pc_factor_mat_solver_package '
    petsc_options_value = ' lu NONZERO superlu_dist '
    dt = 1
    start_time = 0                 
    end_time = 75000             
    automatic_scaling = true  
[]

[Outputs]
    [./exodus]
      type = Exodus
      interval = 1     
      file_base = simulation     
      execute_on = 'initial timestep_end' 
    [../]
    [./checkpoint]
        type = Checkpoint
        time_step_interval = 20
        num_files = 2   
    [../]
[]