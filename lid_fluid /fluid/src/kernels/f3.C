#include "f3.h"

registerMooseObject("fluidApp", f3);

InputParameters
f3::validParams()
{
  InputParameters params = ADKernel::validParams();
  params.addCoupledVar("uy", " y-component of u ");
  return params;
}

f3::f3(const InputParameters & parameters)
  : ADKernel(parameters),
    _uy(adCoupledValue("uy"))
{
}

ADReal
f3::computeQpResidual()
{
  ADReal ux = _u[_qp];    
  ADReal uy = _uy[_qp];  
  return  ux*_grad_u[_qp](0)* _test[_i][_qp] + uy*_grad_u[_qp](1)* _test[_i][_qp] ;
}