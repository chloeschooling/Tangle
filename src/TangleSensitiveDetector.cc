#include "TangleSensitiveDetector.hh"

#include "TangleRunAction.hh"

#include "G4Step.hh"
#include "G4VProcess.hh"

TangleSensitiveDetector::TangleSensitiveDetector
(const G4String& name)
: G4VSensitiveDetector(name)
{}

void TangleSensitiveDetector::Initialize(G4HCofThisEvent*)
{
  f1stComptonScatteringAnnihilationPhotonFound = false;
  f2ndComptonScatteringAnnihilationPhotonFound = false;
}

void TangleSensitiveDetector::EndOfEvent(G4HCofThisEvent*)
{}

G4bool TangleSensitiveDetector::ProcessHits(G4Step* step,
                                            G4TouchableHistory*)
{
  G4Track* track = step->GetTrack();
  const G4VProcess* creatorProcess = track->GetCreatorProcess();
  G4cout << "creatorProcess: ";
  if (creatorProcess) {
    G4cout << creatorProcess->GetProcessName();
  } else {
    G4cout << "NULL";
  }
  G4cout << G4endl;

  G4StepPoint* preStepPoint = step->GetPreStepPoint();
  const G4VProcess* preProcessDefinedStep = preStepPoint->GetProcessDefinedStep();
  G4cout << "preProcessDefinedStep: ";
  if (preProcessDefinedStep) {
    G4cout << preProcessDefinedStep->GetProcessName();
  } else {
    G4cout << "NULL";
  }
  G4cout << G4endl;

  G4StepPoint* postStepPoint = step->GetPostStepPoint();
  const G4VProcess* postProcessDefinedStep = postStepPoint->GetProcessDefinedStep();
  G4cout << "postProcessDefinedStep: ";
  if (postProcessDefinedStep) {
    G4cout << postProcessDefinedStep->GetProcessName();
  } else {
    G4cout << "NULL";
  }
  G4cout << G4endl;

  if (creatorProcess == nullptr) return true;
//  if (preProcessDefinedStep == nullptr) return true;
  if (postProcessDefinedStep == nullptr) return true;

  if (creatorProcess->GetProcessName() == "annihil" &&
      postProcessDefinedStep->GetProcessName() == "compt") {
    // This is an annihilation photon that Compton scatters.
    if (!f1stComptonScatteringAnnihilationPhotonFound) {
      if (track->GetCurrentStepNumber() == 1) {
        f1stComptonScatteringAnnihilationPhotonFound = true;
        // This is the first step of the first photon
        G4cout << "1st photon found: trackID: " << track->GetTrackID() << G4endl;
        // We want electron from the Compton process.
      }
    } else {
      // First photon found so look for second
      if (!f2ndComptonScatteringAnnihilationPhotonFound) {
        if (track->GetCurrentStepNumber() == 1) {
          f1stComptonScatteringAnnihilationPhotonFound = true;
          // This is the first step of the second photon
          G4cout << "2nd photon found: trackID: " << track->GetTrackID() << G4endl;
          // We want electron from the Compton process.
        }
      } else {
        if (track->GetCurrentStepNumber() == 1) {
          G4cout << "Mmmm: More than one annihilation?" << G4endl;
        }
      }
    }
  }

  return true;
}
