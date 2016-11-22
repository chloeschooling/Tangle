#ifndef AnnihilationPhotonsSteppingAction_hh
#define AnnihilationPhotonsSteppingAction_hh

#include "TangleVSteppingAction.hh"

#include "TangleRunAction.hh"
#include "G4ThreeVector.hh"

class AnnihilationPhotonsSteppingAction: public TangleVSteppingAction
{
public:
  AnnihilationPhotonsSteppingAction(TangleRunAction*);
  virtual void BeginOfEventAction();
  virtual void UserSteppingAction(const G4Step*);
  virtual void EndOfEventAction();

private:
  TangleRunAction* fpRunAction;
  G4bool fComptonScatteringAnnihilationPhotonFound1;
  G4bool fComptonScatteringAnnihilationPhotonFound2;
  G4int fAnnihilationPhotonTrackID1;
  G4int fAnnihilationPhotonTrackID2;
  TangleRunAction::Data data;
};

#endif
