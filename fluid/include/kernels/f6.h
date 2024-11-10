#pragma once
#include "ADKernel.h"


class f6 : public ADKernel
{
public:
  static InputParameters validParams();

  f6(const InputParameters & parameters);

protected:
  ADReal computeQpResidual() override;

  const ADVariableValue & _p;
};
