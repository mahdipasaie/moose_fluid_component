#pragma once
#include "ADKernel.h"

class f3 : public ADKernel
{
public:
  static InputParameters validParams();

  f3(const InputParameters & parameters);

protected:
  /// Computes the residual for the term involving ux and its derivatives
  ADReal computeQpResidual() override;

  /// y-component of the velocity field
  const ADVariableValue & _uy;
};
