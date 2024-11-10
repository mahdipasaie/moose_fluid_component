#pragma once
#include "ADKernelValue.h"

/**
 * f4 Kernel
 *
 * This kernel computes the term ux * (∂uy/∂x) + uy * (∂uy/∂y),
 * where ux and uy are the x- and y-components of the velocity field `u`.
 */
class f4 : public ADKernelValue
{
public:
  static InputParameters validParams();

  f4(const InputParameters & parameters);

protected:
  /// Computes the residual for the term involving ux and derivatives of uy
  ADReal precomputeQpResidual() override;

  /// x-component of the velocity field
  const ADVariableValue & _ux;
};
