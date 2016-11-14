#include "TangleActionInitialization.hh"

#include "TanglePrimaryGeneratorAction.hh"
#include "TangleRunAction.hh"
#include "TangleEventAction.hh"
#include "OnePhotonSteppingAction.hh"

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
  TangleRunAction* tangleRunAction = new TangleRunAction;

  OnePhotonSteppingAction* onePhotonSteppingAction =
  new OnePhotonSteppingAction(tangleRunAction);

  TangleEventAction* tangleEventAction =
  new TangleEventAction(tangleRunAction, onePhotonSteppingAction);

  SetUserAction(new TanglePrimaryGeneratorAction);
  SetUserAction(tangleRunAction);
  SetUserAction(tangleEventAction);
  SetUserAction(onePhotonSteppingAction);
}
