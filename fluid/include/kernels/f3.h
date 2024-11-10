#pragma once
#include "ADKernelValue.h"

class f3 : public ADKernelValue
{
public:
  static InputParameters validParams();

  f3(const InputParameters & parameters);

protected:
  /// Computes the residual for the term involving ux and its derivatives
  ADReal precomputeQpResidual() override;

  /// y-component of the velocity field
  const ADVariableValue & _uy;
};
