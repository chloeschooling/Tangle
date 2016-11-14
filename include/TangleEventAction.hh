#ifndef TangleEventAction_hh
#define TangleEventAction_hh

#include "G4UserEventAction.hh"
#include "globals.hh"

class TangleRunAction;
class OnePhotonSteppingAction;

class TangleEventAction : public G4UserEventAction
{
public:

  TangleEventAction
  (TangleRunAction* runAction,
   OnePhotonSteppingAction* onePhotonSteppingAction);

  virtual ~TangleEventAction();

  virtual void BeginOfEventAction(const G4Event*);
  virtual void EndOfEventAction(const G4Event*);

private:

  TangleRunAction* fRunAction;
  OnePhotonSteppingAction* fOnePhotonSteppingAction;
};

#endif
