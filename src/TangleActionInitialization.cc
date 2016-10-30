#include "TangleActionInitialization.hh"

#include "TanglePrimaryGeneratorAction.hh"
#include "TangleRunAction.hh"

TangleActionInitialization::TangleActionInitialization()
{}

TangleActionInitialization::~TangleActionInitialization()
{}

void TangleActionInitialization::BuildForMaster() const
{
  SetUserAction(new TangleRunAction);
}

void TangleActionInitialization::Build() const
{
  SetUserAction(new TanglePrimaryGeneratorAction);
  SetUserAction(new TangleRunAction);
}
