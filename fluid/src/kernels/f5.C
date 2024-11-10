#include "f5.h"

registerMooseObject("fluidApp", f5);

InputParameters
f5::validParams()
{
  InputParameters params = ADKernelValue::validParams();
  params.addCoupledVar("ux", " x-component of u ");
  params.addCoupledVar("uy", " y-component of u ");
  return params;
}

f5::f5(const InputParameters & parameters)
  : ADKernelValue(parameters),
    _grad_u_x(adCoupledGradient("ux")),
    _grad_u_y(adCoupledGradient("uy"))
{
}

ADReal
f5::precomputeQpResidual()
{
  ADReal dux = _grad_u_x[_qp](0);
  ADReal duy = _grad_u_y[_qp](1);
  return  dux + duy ;
}