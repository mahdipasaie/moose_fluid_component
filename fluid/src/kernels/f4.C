#include "f4.h"

registerMooseObject("fluidApp", f4);

InputParameters
f4::validParams()
{
  InputParameters params = ADKernelValue::validParams();
  params.addCoupledVar("ux", " x-component of u ");
  return params;
}

f4::f4(const InputParameters & parameters)
  : ADKernelValue(parameters),
    _ux(adCoupledValue("ux"))
{
}

ADReal
f4::precomputeQpResidual()
{
  ADReal uy = _u[_qp];    
  ADReal ux = _ux[_qp];    
  return ux*_grad_u[_qp](0) + uy*_grad_u[_qp](1)  ;
}