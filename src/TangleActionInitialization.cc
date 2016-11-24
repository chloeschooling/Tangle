#include "TangleActionInitialization.hh"

#include "TanglePrimaryGeneratorAction.hh"
#include "TangleRunAction.hh"
#include "TangleEventAction.hh"
//#include "TangleStackingAction.hh"
#include "OnePhotonSteppingAction.hh"
#include "AnnihilationPhotonsSteppingAction.hh"

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

//  OnePhotonSteppingAction* steppingAction =
//  new OnePhotonSteppingAction(tangleRunAction);
  AnnihilationPhotonsSteppingAction* steppingAction =
  new AnnihilationPhotonsSteppingAction(tangleRunAction);

  TangleEventAction* tangleEventAction =
  new TangleEventAction(steppingAction);

  SetUserAction(new TanglePrimaryGeneratorAction);
  SetUserAction(tangleRunAction);
  SetUserAction(tangleEventAction);
  SetUserAction(steppingAction);
//  SetUserAction(new TangleStackingAction);
}
