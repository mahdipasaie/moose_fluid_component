#include "f7.h"

registerMooseObject("fluidApp", f7);

InputParameters
f7::validParams()
{
  InputParameters params = ADKernel::validParams();
  params.addClassDescription("< -p * I , grad(test) >  ");
  params.addRequiredCoupledVar("p", "Pressure ");
  return params;
}

f7::f7(const InputParameters & parameters)
  : ADKernel(parameters),
    _p(adCoupledValue("p"))
{
}

ADReal
f7::computeQpResidual()
{
  return -_p[_qp]* _grad_test[_i][_qp](1) ;
}