#ifndef OnePhotonSteppingAction_hh
#define OnePhotonSteppingAction_hh

#include "TangleVSteppingAction.hh"
#include "globals.hh"

class TangleRunAction;

class OnePhotonSteppingAction : public TangleVSteppingAction
{
public:
  OnePhotonSteppingAction(TangleRunAction* runAction);
  virtual void UserSteppingAction(const G4Step*);
  virtual void ResetAtBeginOfEvent();

private:
  TangleRunAction* fpRunAction;
  G4bool fComptonScatteringFound;
};

#endif
