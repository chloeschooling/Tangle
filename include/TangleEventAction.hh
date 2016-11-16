#ifndef TangleEventAction_hh
#define TangleEventAction_hh

#include "G4UserEventAction.hh"
#include "globals.hh"

class TangleRunAction;
class TangleVSteppingAction;

class TangleEventAction : public G4UserEventAction
{
public:

  TangleEventAction(TangleVSteppingAction*);

  virtual ~TangleEventAction();

  virtual void BeginOfEventAction(const G4Event*);
  virtual void EndOfEventAction(const G4Event*);

private:

  TangleVSteppingAction* fTangleVSteppingAction;
};

#endif
