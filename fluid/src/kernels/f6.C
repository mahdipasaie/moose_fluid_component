#include "f6.h"

registerMooseObject("fluidApp", f6);

InputParameters
f6::validParams()
{
  InputParameters params = ADKernel::validParams();
  params.addClassDescription("< -p * I , grad(test) >  ");
  params.addRequiredCoupledVar("p", "Pressure ");
  return params;
}

f6::f6(const InputParameters & parameters)
  : ADKernel(parameters),
    _p(adCoupledValue("p"))
{
}

ADReal
f6::computeQpResidual()
{
  return -_p[_qp]* _grad_test[_i][_qp](0) ;
}