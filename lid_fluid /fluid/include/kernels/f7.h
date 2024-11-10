#pragma once
#include "ADKernel.h"


class f7 : public ADKernel
{
public:
  static InputParameters validParams();

  f7(const InputParameters & parameters);

protected:
  ADReal computeQpResidual() override;

  const ADVariableValue & _p;
};
