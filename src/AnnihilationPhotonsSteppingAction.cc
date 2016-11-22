#include "AnnihilationPhotonsSteppingAction.hh"

#include "TangleRunAction.hh"

#include "G4Step.hh"
#include "G4VProcess.hh"
#include "G4MTRunManager.hh"
#include "G4EventManager.hh"
#include "G4TrackingManager.hh"
#include "G4SteppingManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

AnnihilationPhotonsSteppingAction::AnnihilationPhotonsSteppingAction
(TangleRunAction* runAction)
: fpRunAction(runAction)
{}

void AnnihilationPhotonsSteppingAction::BeginOfEventAction()
{
  fComptonScatteringAnnihilationPhotonFound1 = false;
  fComptonScatteringAnnihilationPhotonFound2 = false;
}

void AnnihilationPhotonsSteppingAction::EndOfEventAction()
{
}

void AnnihilationPhotonsSteppingAction::UserSteppingAction(const G4Step* step)
{
  G4Track* track = step->GetTrack();
  const G4VProcess* creatorProcess = track->GetCreatorProcess();
  if (creatorProcess == nullptr) return;

  G4StepPoint* preStepPoint = step->GetPreStepPoint();

  G4StepPoint* postStepPoint = step->GetPostStepPoint();
  const G4VProcess* postProcessDefinedStep = postStepPoint->GetProcessDefinedStep();
  if (postProcessDefinedStep == nullptr) return;

  if (creatorProcess->GetProcessName() == "annihil" &&
      postProcessDefinedStep->GetProcessName() == "compt") {
    // This is an annihilation photon that Compton scatters.
    if (!fComptonScatteringAnnihilationPhotonFound1) {
      if (track->GetCurrentStepNumber() == 1) {
        fComptonScatteringAnnihilationPhotonFound1 = true;
        // This is the first step of the first photon
        fAnnihilationPhotonTrackID1 = track->GetTrackID();
//        G4cout
//        << "\n  1st photon found: trackID: " << track->GetTrackID()
//        << "\n  preStepPointPosition: " << preStepPoint->GetPosition()
//        << "\n  postStepPointPosition: " << postStepPoint->GetPosition()
//        << "\n  preStepPointMomentum: " << preStepPoint->GetMomentum()
//        << "\n  postStepPointMomentum: " << postStepPoint->GetMomentum()
//        << "\n  preStepPolarisation: " << preStepPoint->GetPolarization()
//        << "\n  postStepPolarisation: " << postStepPoint->GetPolarization()
//        << G4endl;
//        G4cout << "\n  Scattering plane: "
//        << preStepPoint->GetMomentum().cross(postStepPoint->GetMomentum()).unit()
//        << G4endl;
        const G4ThreeVector annihilation_z_axis = preStepPoint->GetMomentumDirection();
        // Make y' perpendicular to global x-axis.
        const G4ThreeVector annihilation_y_axis = (annihilation_z_axis.cross(G4ThreeVector(1,0,0))).unit();
//        const G4ThreeVector annihilation_x_axis = annihilation_y_axis.cross(annihilation_z_axis);
        const G4ThreeVector scatteringPlane = postStepPoint->GetMomentum().cross(annihilation_z_axis).unit();
//        G4cout << "\n  Scattering plane1: " << scatteringPlane << G4endl;
        G4double phi1 = std::acos(scatteringPlane*annihilation_y_axis);
//        G4cout << "\n  phi1: " << phi1 << G4endl;
        data.fPhi1 = phi1;
      }
    } else {
      // First photon found so look for second
      if (!fComptonScatteringAnnihilationPhotonFound2) {
        if (track->GetCurrentStepNumber() == 1) {
          fComptonScatteringAnnihilationPhotonFound2 = true;
          // This is the first step of the second photon
          fAnnihilationPhotonTrackID2 = track->GetTrackID();
          if (std::abs(fAnnihilationPhotonTrackID2 - fAnnihilationPhotonTrackID1) != 1)
          {
            G4cout <<
            "\n  Annihilation photons do not have adjacent track id's"
            "\n  The two track id's are: "
            << fAnnihilationPhotonTrackID1
            << ", " << fAnnihilationPhotonTrackID2
            << G4endl;
            fComptonScatteringAnnihilationPhotonFound2 = false;
            return;
          }
//          G4cout
//          << "\n  2nd photon found: trackID: " << track->GetTrackID()
//          << "\n  preStepPointPosition: " << preStepPoint->GetPosition()
//          << "\n  postStepPointPosition: " << postStepPoint->GetPosition()
//          << "\n  preStepPointMomentum: " << preStepPoint->GetMomentum()
//          << "\n  postStepPointMomentum: " << postStepPoint->GetMomentum()
//          << "\n  preStepPolarisation: " << preStepPoint->GetPolarization()
//          << "\n  postStepPolarisation: " << postStepPoint->GetPolarization()
//          << G4endl;
//          G4cout << "\n  Scattering plane: "
//          << preStepPoint->GetMomentum().cross(postStepPoint->GetMomentum()).unit()
//          << G4endl;
          const G4ThreeVector annihilation_z_axis = preStepPoint->GetMomentumDirection();
          // Make y' perpendicular to global x-axis.
          const G4ThreeVector annihilation_y_axis = (annihilation_z_axis.cross(G4ThreeVector(1,0,0))).unit();
          //        const G4ThreeVector annihilation_x_axis = annihilation_y_axis.cross(annihilation_z_axis);
          const G4ThreeVector scatteringPlane = postStepPoint->GetMomentum().cross(annihilation_z_axis).unit();
//          G4cout << "\n  Scattering plane2: " << scatteringPlane << G4endl;
          G4double phi2 = std::acos(scatteringPlane*annihilation_y_axis);
//          G4cout << "\n  phi2: " << phi2 << G4endl;
          data.fPhi2 = phi2;

          fpRunAction->RecordData(data);

          //Reset for other possible annihilations in this event.
          fComptonScatteringAnnihilationPhotonFound1 = false;
          fComptonScatteringAnnihilationPhotonFound2 = false;
        }
      }
    }
  }

  return;
}
