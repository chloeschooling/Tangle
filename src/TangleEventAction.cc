#include "TangleEventAction.hh"

#include "TangleRunAction.hh"
#include "OnePhotonSteppingAction.hh"

TangleEventAction::TangleEventAction
(TangleRunAction* runAction,
 OnePhotonSteppingAction* onePhotonSteppingAction)
: fRunAction(runAction)
, fOnePhotonSteppingAction(onePhotonSteppingAction)
{}

TangleEventAction::~TangleEventAction()
{}

void TangleEventAction::BeginOfEventAction(const G4Event*)
{
  fOnePhotonSteppingAction->ResetAtBeginOfEvent();
}

void TangleEventAction::EndOfEventAction(const G4Event*)
{   
}
