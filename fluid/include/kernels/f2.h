#pragma once
#include "ADKernel.h"

/**
 * f2 Kernel
 *
 * This kernel implements the y-component of the weak form inner product:
 * < -p * I + mu * grad(u), grad(test) >, representing the combined pressure
 * and viscous stress terms in the Navier-Stokes equations.
 */
class f2 : public ADKernel
{
public:
  static InputParameters validParams();

  f2(const InputParameters & parameters);

protected:
  ADReal computeQpResidual() override;

  const ADVariableValue & _p;
  const ADMaterialProperty<Real> & _mu;
  const ADMaterialProperty<Real> & _rho;
};
