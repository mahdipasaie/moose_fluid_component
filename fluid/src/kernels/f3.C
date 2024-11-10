#include "f3.h"

registerMooseObject("fluidApp", f3);

InputParameters
f3::validParams()
{
  InputParameters params = ADKernelValue::validParams();
  params.addCoupledVar("uy", " y-component of u ");
  return params;
}

f3::f3(const InputParameters & parameters)
  : ADKernelValue(parameters),
    _uy(adCoupledValue("uy"))
{
}

ADReal
f3::precomputeQpResidual()
{
  ADReal ux = _u[_qp];    
  ADReal uy = _uy[_qp];  
  return  ux*_grad_u[_qp](0) + uy*_grad_u[_qp](1) ;
}