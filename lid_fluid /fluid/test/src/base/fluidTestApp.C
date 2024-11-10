//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
#include "fluidTestApp.h"
#include "fluidApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "MooseSyntax.h"
#include "ModulesApp.h"

InputParameters
fluidTestApp::validParams()
{
  InputParameters params = fluidApp::validParams();
  return params;
}

fluidTestApp::fluidTestApp(InputParameters parameters) : MooseApp(parameters)
{
  fluidTestApp::registerAll(
      _factory, _action_factory, _syntax, getParam<bool>("allow_test_objects"));
}

fluidTestApp::~fluidTestApp() {}

void
fluidTestApp::registerAll(Factory & f, ActionFactory & af, Syntax & s, bool use_test_objs)
{
  fluidApp::registerAll(f, af, s);
  if (use_test_objs)
  {
    Registry::registerObjectsTo(f, {"fluidTestApp"});
    Registry::registerActionsTo(af, {"fluidTestApp"});
  }
}

void
fluidTestApp::registerApps()
{
  registerApp(fluidApp);
  registerApp(fluidTestApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
// External entry point for dynamic application loading
extern "C" void
fluidTestApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  fluidTestApp::registerAll(f, af, s);
}
extern "C" void
fluidTestApp__registerApps()
{
  fluidTestApp::registerApps();
}
