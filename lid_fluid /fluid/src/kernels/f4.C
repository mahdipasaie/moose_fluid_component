#include "f4.h"

registerMooseObject("fluidApp", f4);

InputParameters
f4::validParams()
{
  InputParameters params = ADKernel::validParams();
  params.addCoupledVar("ux", " x-component of u ");
  return params;
}

f4::f4(const InputParameters & parameters)
  : ADKernel(parameters),
    _ux(adCoupledValue("ux"))
{
}

ADReal
f4::computeQpResidual()
{
  ADReal uy = _u[_qp];    
  ADReal ux = _ux[_qp];    
  return ux*_grad_u[_qp](0)* _test[_i][_qp] + uy*_grad_u[_qp](1)* _test[_i][_qp]  ;
}