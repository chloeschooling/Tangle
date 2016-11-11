#ifndef PHANTOMSENSITIVEDETECTOR_HH
#define PHANTOMSENSITIVEDETECTOR_HH

#include "G4VSensitiveDetector.hh"

class TangleRunAction;

class TangleSensitiveDetector : public G4VSensitiveDetector
{
public:
  TangleSensitiveDetector(const G4String& name, TangleRunAction*);
  void Initialize(G4HCofThisEvent*);
  void EndOfEvent(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);

private:
  TangleRunAction* fpRunAction;
  G4bool fComptonScatteringAnnihilationPhotonFound1;
  G4bool fComptonScatteringAnnihilationPhotonFound2;
  G4ThreeVector fPhotonOriginPosition1;
  G4ThreeVector fPhotonOriginPosition2;
  G4ThreeVector fPhotonPolarisation1;
  G4ThreeVector fPhotonPolarisation2;
  G4ThreeVector fComptonScatteredPhotonPosition1;
  G4ThreeVector fComptonScatteredPhotonPosition2;
  G4ThreeVector fComptonScatteredPhotonMomentum1;
  G4ThreeVector fComptonScatteredPhotonMomentum2;
};

#endif
