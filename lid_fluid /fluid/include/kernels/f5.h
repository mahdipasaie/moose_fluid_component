#pragma once
#include "ADKernel.h"

/**
 * f5 Kernel
 *
 * This kernel computes the term ∂ux/∂x + ∂uy/∂y, which represents the divergence
 * of the velocity field `u` in a 2D flow.
 */
class f5 : public ADKernel
{
public:
  static InputParameters validParams();

  f5(const InputParameters & parameters);

protected:
  /// Computes the residual for the divergence of the velocity field
  ADReal computeQpResidual() override;
  /// Gradient of the x-component of the velocity field
  const ADVariableGradient & _grad_u_x;
  /// Gradient of the y-component of the velocity field
  const ADVariableGradient & _grad_u_y;
};
