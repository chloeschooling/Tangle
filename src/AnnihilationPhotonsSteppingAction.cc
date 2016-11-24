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

#define AnnihilationPhotonsSteppingActionPrinting
#define AnnihilationPhotonsSteppingActionConsistencyCheck

void AnnihilationPhotonsSteppingAction::UserSteppingAction(const G4Step* step)
{
  // Use statics to remember things about the first photon.
  static G4int parentID1 = 0.;
  static G4double theta1 = 0.;
  static G4double phi1 = 0.;
  static G4ThreeVector annihilation_z_axis;
  static G4ThreeVector annihilation_y_axis;
//  static G4ThreeVector annihilation_x_axis;
  static G4ThreeVector scatteringPlane;

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
        parentID1 = track->GetParentID();
        annihilation_z_axis = preStepPoint->GetMomentumDirection();
        theta1 = std::acos(postStepPoint->GetMomentumDirection()*annihilation_z_axis);
        // Make y' perpendicular to global x-axis.
        annihilation_y_axis = (annihilation_z_axis.cross(G4ThreeVector(1,0,0))).unit();
        //        annihilation_x_axis = annihilation_y_axis.cross(annihilation_z_axis);
        scatteringPlane = postStepPoint->GetMomentum().cross(annihilation_z_axis).unit();
        phi1 = std::acos(scatteringPlane*annihilation_y_axis);
#ifdef AnnihilationPhotonsSteppingActionPrinting
        G4cout
        << "\n  1st photon found: track ID: " << track->GetTrackID()
        << "\n  parent ID: " << track->GetParentID()
        << "\n  preStepPointPosition: " << preStepPoint->GetPosition()
        << "\n  postStepPointPosition: " << postStepPoint->GetPosition()
        << "\n  preStepPointMomentum: " << preStepPoint->GetMomentum()
        << "\n  postStepPointMomentum: " << postStepPoint->GetMomentum()
        << "\n  preStepPolarisation: " << preStepPoint->GetPolarization()
        << "\n  postStepPolarisation: " << postStepPoint->GetPolarization()
        << "\n  Scattering plane: " << scatteringPlane
        << "\n  phi1: " << phi1
        << G4endl;
#endif  // AnnihilationPhotonsSteppingActionPrinting
        data.fPhi1 = phi1;
      }
    } else {
      // First photon found so look for second
      if (!fComptonScatteringAnnihilationPhotonFound2) {
        if (track->GetCurrentStepNumber() == 1) {
          fComptonScatteringAnnihilationPhotonFound2 = true;
          // This is the first step of the second photon
#ifdef AnnihilationPhotonsSteppingActionConsistencyCheck
          if (track->GetParentID() != parentID1)
          {
            G4cout <<
            "\n  Annihilation photons do not have the same parent ID"
            << G4endl;
            fComptonScatteringAnnihilationPhotonFound2 = false;
            return;
          }
          if (preStepPoint->GetMomentumDirection() * annihilation_z_axis > -0.999999) {
            G4cout <<
            "\n  Annihilation photons not in opposite directions"
            << G4endl;
            fComptonScatteringAnnihilationPhotonFound2 = false;
            return;
          }
#endif // AnnihilationPhotonsSteppingActionConsistencyCheck
#if defined AnnihilationPhotonsSteppingActionPrinting || defined AnnihilationPhotonsSteppingActionConsistencyCheck
          const G4ThreeVector originalScatteringPlane2 = postStepPoint->GetMomentum().cross(annihilation_z_axis).unit();
          const G4double originalPhi2 = std::acos(originalScatteringPlane2*annihilation_y_axis);
#endif  // defined AnnihilationPhotonsSteppingActionPrinting || defined AnnihilationPhotonsSteppingActionConsistencyCheck
#ifdef AnnihilationPhotonsSteppingActionPrinting
          G4cout
          << "\n  2nd photon found: track ID: " << track->GetTrackID()
          << "\n  parent ID: " << track->GetParentID()
          << "\n  preStepPointPosition: " << preStepPoint->GetPosition()
          << "\n  postStepPointPosition: " << postStepPoint->GetPosition()
          << "\n  preStepPointMomentum: " << preStepPoint->GetMomentum()
          << "\n  postStepPointMomentum: " << postStepPoint->GetMomentum()
          << "\n  preStepPolarisation: " << preStepPoint->GetPolarization()
          << "\n  postStepPolarisation: " << postStepPoint->GetPolarization()
          << "\n  Scattering plane (local): "
          << preStepPoint->GetMomentum().cross(postStepPoint->GetMomentum()).unit()
          << "\n  original scattering plane2 (wrt photon1 z-axis): " << originalScatteringPlane2
          << "\n  originalPhi2 (wrt photon1 z-axis): " << originalPhi2
          << G4endl;
#endif  // AnnihilationPhotonsSteppingActionPrinting

          // OK - testing.  Set theta and phi of the Compton scatter of the second photon.
          G4double theta2 = 90.*degree;
          G4double phi2 = 180.*degree;
          G4ThreeVector newMomentumDirection;
          newMomentumDirection.setRThetaPhi(1.,theta2,phi2);


          track->SetMomentumDirection(G4ThreeVector(0,0,1));

#ifdef AnnihilationPhotonsSteppingActionConsistencyCheck
          const G4ThreeVector newScatteringPlane = track->GetMomentumDirection().cross(annihilation_z_axis).unit();
          G4double newPhi2 = std::acos(newScatteringPlane*annihilation_y_axis);
          if (std::abs(newPhi2 - phi2) > 0.00001) {
            G4cout
            << "\n  Inconsistent calculation of phi"
            << "\n  original scattering plane2 (wrt photon1 z-axis): " << originalScatteringPlane2
            << "\n  New scattering plane2 (wrt photon1 z-axis): " << newScatteringPlane
            << "\n  originalPhi2 (wrt photon1 z-axis): " << originalPhi2
            << "\n  newPhi2 (wrt photon1 z-axis): " << newPhi2
            << G4endl;
          }
#endif // AnnihilationPhotonsSteppingActionConsistencyCheck


          data.fPhi2 = phi2;
          fpRunAction->RecordData(data);

          //Reset for further possible annihilations in this event.
          fComptonScatteringAnnihilationPhotonFound1 = false;
          fComptonScatteringAnnihilationPhotonFound2 = false;
        }
      }
    }
  }

  return;
}
