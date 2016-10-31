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
(const G4String& name, TangleRunAction* runAction)
: G4VSensitiveDetector(name)
, fpRunAction(runAction)
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
    const G4ThreeVector annihilation_z_axis = (f1stComptonElectronPosition - f1stPhotonOriginPosition).unit();
    G4ThreeVector difference = annihilation_z_axis - (f2ndPhotonOriginPosition - f2ndComptonElectronPosition).unit();
    if (difference.mag() > 0.0000001) {
      G4cerr << "Axis mis-alignment" << G4endl;
//      abort();
    }
    const G4ThreeVector annihilation_y_axis = (annihilation_z_axis.cross(G4ThreeVector(1.,0.,0.))).unit();
    const G4ThreeVector annihilation_x_axis = annihilation_y_axis.cross(annihilation_z_axis);
    const G4ThreeVector v1 = f1stComptonElectronMomentum.cross(annihilation_z_axis);
    const G4double& v1_mag = v1.mag();
    G4double phi1 = std::acos(v1*annihilation_y_axis/v1_mag);
//    G4cout << "phi1: " << phi1 << G4endl;
    const G4ThreeVector v2 = f2ndComptonElectronMomentum.cross(annihilation_z_axis);
    const G4double& v2_mag = v2.mag();
    G4double phi2 = std::acos(v2*annihilation_y_axis/v2_mag);
//    G4cout << "phi2: " << phi2 << G4endl;
    fpRunAction->RecordDeltaPhi(TangleRunAction::Data(phi2,phi1));
  } else {
    G4cout << "No double Comptons" << G4endl;
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
//        G4cout
//        << "1st photon found: trackID: " << track->GetTrackID()
//        << ", preStepPointPosition: " << preStepPoint->GetPosition()
//        << ", postStepPointPosition: " << postStepPoint->GetPosition()
//        << ", position: " << track->GetPosition()
//        << ", momentum: " << track->GetMomentum()
//        << G4endl;
        // We want the electron from the Compton process.
        G4EventManager* eventManager = G4EventManager::GetEventManager();
        G4TrackingManager* trackingManager = eventManager->GetTrackingManager();
        G4SteppingManager* steppingManager = trackingManager->GetSteppingManager();
        const G4TrackVector* secondaries = steppingManager->GetSecondary();
        if (secondaries->size() != 1) {
          G4cerr << "oops" << G4endl;
//          abort();
        }
        f1stPhotonOriginPosition = preStepPoint->GetPosition();
        f1stComptonElectronPosition = (*secondaries)[0]->GetPosition();
        f1stComptonElectronMomentum = (*secondaries)[0]->GetMomentum();
        if (f1stComptonElectronPosition != postStepPoint->GetPosition()) {
          G4cout << "Positions don't match" << G4endl;
//          abort();
        }
//        G4cout
//        << "Electron position: " << f1stComptonElectronPosition
//        << ", momentum: " << f1stComptonElectronMomentum
//        << G4endl;
      }
    } else {
      // First photon found so look for second
      if (!f2ndComptonScatteringAnnihilationPhotonFound) {
        if (track->GetCurrentStepNumber() == 1) {
          f2ndComptonScatteringAnnihilationPhotonFound = true;
          // This is the first step of the second photon
//          G4cout
//          << "2nd photon found: trackID: " << track->GetTrackID()
//          << ", preStepPointPosition: " << preStepPoint->GetPosition()
//          << ", postStepPointPosition: " << postStepPoint->GetPosition()
//          << ", position: " << track->GetPosition()
//          << ", momentum: " << track->GetMomentum()
//          << G4endl;
          // We want the electron from the Compton process.
          G4EventManager* eventManager = G4EventManager::GetEventManager();
          G4TrackingManager* trackingManager = eventManager->GetTrackingManager();
          G4SteppingManager* steppingManager = trackingManager->GetSteppingManager();
          const G4TrackVector* secondaries = steppingManager->GetSecondary();
          if (secondaries->size() != 1) {
            G4cerr << "oops" << G4endl;
//            abort();
          }
          f2ndPhotonOriginPosition = preStepPoint->GetPosition();
          f2ndComptonElectronPosition = (*secondaries)[0]->GetPosition();
          f2ndComptonElectronMomentum = (*secondaries)[0]->GetMomentum();
          if (f2ndComptonElectronPosition != postStepPoint->GetPosition()) {
            G4cout << "Positions don't match" << G4endl;
//            abort();
          }
//          G4cout
//          << "Electron position: " << f2ndComptonElectronPosition
//          << ", momentum: " << f2ndComptonElectronMomentum
//          << G4endl;
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
