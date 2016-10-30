#include "TangleSensitiveDetector.hh"

#include "TangleRunAction.hh"

#include "G4Step.hh"
#include "G4VProcess.hh"
#include "G4MTRunManager.hh"
#include "G4EventManager.hh"
#include "G4TrackingManager.hh"
#include "G4SteppingManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

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
{
  if (f1stComptonScatteringAnnihilationPhotonFound &&
      f2ndComptonScatteringAnnihilationPhotonFound) {
    G4double phi = 0.;
  }
}

G4bool TangleSensitiveDetector::ProcessHits(G4Step* step,
                                            G4TouchableHistory*)
{
  G4Track* track = step->GetTrack();
  const G4VProcess* creatorProcess = track->GetCreatorProcess();
  if (creatorProcess == nullptr) return true;

  G4StepPoint* preStepPoint = step->GetPreStepPoint();

  G4StepPoint* postStepPoint = step->GetPostStepPoint();
  const G4VProcess* postProcessDefinedStep = postStepPoint->GetProcessDefinedStep();
  if (postProcessDefinedStep == nullptr) return true;

  if (creatorProcess->GetProcessName() == "annihil" &&
      postProcessDefinedStep->GetProcessName() == "compt") {
    // This is an annihilation photon that Compton scatters.
    if (!f1stComptonScatteringAnnihilationPhotonFound) {
      if (track->GetCurrentStepNumber() == 1) {
        f1stComptonScatteringAnnihilationPhotonFound = true;
        // This is the first step of the first photon
        G4cout
        << "1st photon found: trackID: " << track->GetTrackID()
        << ", preStepPointPosition: " << preStepPoint->GetPosition()
        << ", postStepPointPosition: " << postStepPoint->GetPosition()
        << ", position: " << track->GetPosition()
        << ", momentum: " << track->GetMomentum()
        << G4endl;
        // We want the electron from the Compton process.
        G4EventManager* eventManager = G4EventManager::GetEventManager();
        G4TrackingManager* trackingManager = eventManager->GetTrackingManager();
        G4SteppingManager* steppingManager = trackingManager->GetSteppingManager();
        const G4TrackVector* secondaries = steppingManager->GetSecondary();
        if (secondaries->size() != 1) {
          G4cerr << "oops" << G4endl;
          abort();
        }
        f1stPhotonOriginPosition = preStepPoint->GetPosition();
        f1stComptonElectronPosition = (*secondaries)[0]->GetPosition();
        f1stComptonElectronMomentum = (*secondaries)[0]->GetMomentum();
        if (f1stComptonElectronPosition != postStepPoint->GetPosition()) {
          G4cout << "Positions don't match" << G4endl;
          abort();
        }
        G4cout
        << "Electron position: " << f1stComptonElectronPosition
        << ", momentum: " << f1stComptonElectronMomentum
        << G4endl;
      }
    } else {
      // First photon found so look for second
      if (!f2ndComptonScatteringAnnihilationPhotonFound) {
        if (track->GetCurrentStepNumber() == 1) {
          f1stComptonScatteringAnnihilationPhotonFound = true;
          // This is the first step of the second photon
          G4cout
          << "2nd photon found: trackID: " << track->GetTrackID()
          << ", preStepPointPosition: " << preStepPoint->GetPosition()
          << ", postStepPointPosition: " << postStepPoint->GetPosition()
          << ", position: " << track->GetPosition()
          << ", momentum: " << track->GetMomentum()
          << G4endl;
          // We want the electron from the Compton process.
          G4EventManager* eventManager = G4EventManager::GetEventManager();
          G4TrackingManager* trackingManager = eventManager->GetTrackingManager();
          G4SteppingManager* steppingManager = trackingManager->GetSteppingManager();
          const G4TrackVector* secondaries = steppingManager->GetSecondary();
          if (secondaries->size() != 1) {
            G4cerr << "oops" << G4endl;
            abort();
          }
          f2ndPhotonOriginPosition = preStepPoint->GetPosition();
          f2ndComptonElectronPosition = (*secondaries)[0]->GetPosition();
          f2ndComptonElectronMomentum = (*secondaries)[0]->GetMomentum();
          if (f2ndComptonElectronPosition != postStepPoint->GetPosition()) {
            G4cout << "Positions don't match" << G4endl;
            abort();
          }
          G4cout
          << "Electron position: " << f2ndComptonElectronPosition
          << ", momentum: " << f2ndComptonElectronMomentum
          << G4endl;
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
