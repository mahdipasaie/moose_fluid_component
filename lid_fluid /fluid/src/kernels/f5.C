#include "f5.h"

registerMooseObject("fluidApp", f5);

InputParameters
f5::validParams()
{
  InputParameters params = ADKernel::validParams();
  params.addCoupledVar("ux", " x-component of u ");
  params.addCoupledVar("uy", " y-component of u ");
  return params;
}

f5::f5(const InputParameters & parameters)
  : ADKernel(parameters),
    _grad_u_x(adCoupledGradient("ux")),
    _grad_u_y(adCoupledGradient("uy"))
{
}

ADReal
f5::computeQpResidual()
{
  ADReal dux = _grad_u_x[_qp](0);
  ADReal duy = _grad_u_y[_qp](1);
  return  dux*_test[_i][_qp] + duy*_test[_i][_qp] ;
}