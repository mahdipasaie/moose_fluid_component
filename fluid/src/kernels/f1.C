#include "f1.h"

registerMooseObject("fluidApp", f1);

InputParameters
f1::validParams()
{
  InputParameters params = ADKernel::validParams();
  params.addClassDescription("< -p * I + mu * grad(u), grad(test) > ,  the x-component of this ");
  params.addRequiredCoupledVar("p", "Pressure ");
  params.addParam<MaterialPropertyName>("mu", "mu", "viscosity function");
  params.addParam<MaterialPropertyName>("rho", "rho", "rho function");
  return params;
}

f1::f1(const InputParameters & parameters)
  : ADKernel(parameters),
    _p(adCoupledValue("p")),
    _mu(getADMaterialProperty<Real>("mu")),
    _rho(getADMaterialProperty<Real>("rho"))
{
}

ADReal
f1::computeQpResidual()
{
  const ADReal C = _mu[_qp]/_rho[_qp];

  return C* _grad_u[_qp]* _grad_test[_i][_qp];
}