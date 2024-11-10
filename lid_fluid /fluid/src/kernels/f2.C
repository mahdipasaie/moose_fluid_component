#include "f2.h"

registerMooseObject("fluidApp", f2);

InputParameters
f2::validParams()
{
  InputParameters params = ADKernel::validParams();
  params.addClassDescription("< -p * I + mu * grad(u), grad(test) > ,  the y-component of this ");
  params.addRequiredCoupledVar("p", "Pressure ");
  params.addParam<MaterialPropertyName>("mu", "mu", "viscosity function");
  params.addParam<MaterialPropertyName>("rho", "rho", "rho function");
  return params;
}

f2::f2(const InputParameters & parameters)
  : ADKernel(parameters),
    _p(adCoupledValue("p")),
    _mu(getADMaterialProperty<Real>("mu")),
    _rho(getADMaterialProperty<Real>("rho"))
{
}

ADReal
f2::computeQpResidual()
{
  const ADReal C = _mu[_qp]/_rho[_qp];
  return C* (_grad_u[_qp](0)* _grad_test[_i][_qp](0)+ _grad_u[_qp](1)* _grad_test[_i][_qp](1))  ;
}