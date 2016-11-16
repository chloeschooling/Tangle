#include "TangleEventAction.hh"

#include "TangleRunAction.hh"
#include "TangleVSteppingAction.hh"

TangleEventAction::TangleEventAction
(TangleVSteppingAction* onePhotonSteppingAction)
: fTangleVSteppingAction(onePhotonSteppingAction)
{}

TangleEventAction::~TangleEventAction()
{}

void TangleEventAction::BeginOfEventAction(const G4Event*)
{
  fTangleVSteppingAction->BeginOfEventAction();
}

void TangleEventAction::EndOfEventAction(const G4Event*)
{   
  fTangleVSteppingAction->EndOfEventAction();
}
