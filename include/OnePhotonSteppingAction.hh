#ifndef OnePhotonSteppingAction_hh
#define OnePhotonSteppingAction_hh

#include "G4UserSteppingAction.hh"
#include "globals.hh"

class TangleRunAction;

class OnePhotonSteppingAction : public G4UserSteppingAction
{
public:
  OnePhotonSteppingAction(TangleRunAction* runAction);
  virtual void UserSteppingAction(const G4Step*);
  void ResetAtBeginOfEvent();

private:
  TangleRunAction* fpRunAction;
  G4bool fComptonScatteringFound;
};

#endif
