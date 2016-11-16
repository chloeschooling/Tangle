#ifndef TangleVSteppingAction_hh
#define TangleVSteppingAction_hh

#include "G4UserSteppingAction.hh"
#include "globals.hh"

class TangleVSteppingAction : public G4UserSteppingAction
{
public:
  virtual void BeginOfEventAction() {};
  virtual void EndOfEventAction() {};
};

#endif
