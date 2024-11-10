#include "fluidApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
#include "MooseSyntax.h"

InputParameters
fluidApp::validParams()
{
  InputParameters params = MooseApp::validParams();

  // Do not use legacy material output, i.e., output properties on INITIAL as well as TIMESTEP_END
  params.set<bool>("use_legacy_material_output") = false;

  return params;
}

fluidApp::fluidApp(InputParameters parameters) : MooseApp(parameters)
{
  fluidApp::registerAll(_factory, _action_factory, _syntax);
}

fluidApp::~fluidApp() {}

void
fluidApp::registerAll(Factory & f, ActionFactory & af, Syntax & syntax)
{
  ModulesApp::registerAll(f, af, syntax);
  Registry::registerObjectsTo(f, {"fluidApp"});
  Registry::registerActionsTo(af, {"fluidApp"});

  /* register custom execute flags, action syntax, etc. here */
}

void
fluidApp::registerApps()
{
  registerApp(fluidApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
extern "C" void
fluidApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  fluidApp::registerAll(f, af, s);
}
extern "C" void
fluidApp__registerApps()
{
  fluidApp::registerApps();
}
