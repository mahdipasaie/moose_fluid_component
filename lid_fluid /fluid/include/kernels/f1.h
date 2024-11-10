#pragma once
#include "ADKernel.h"

class f1 : public ADKernel
{
public:
  static InputParameters validParams();

  f1(const InputParameters & parameters);

protected:
  /// Computes the modified gradient vector for the x-component
  ADReal computeQpResidual() override;

  /// Pressure field variable
  const ADVariableValue & _p;
  /// Viscosity property
  const ADMaterialProperty<Real> & _mu;
  /// Density property
  const ADMaterialProperty<Real> & _rho;
};
