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

  G4bool fAnnihilationPhotonFound1;
  G4bool fComptonScatteringAnnihilationPhotonFound1;

  // These data memebers are used to remember things about the first photon.
  G4int fTrackID1;
  G4int fParentID1;
  G4double fCosTheta1;
  G4double fTheta1;
  G4double fPhi1;
  G4ThreeVector fPhoton1_z_axis;

  TangleRunAction::Data data;
};

#endif
