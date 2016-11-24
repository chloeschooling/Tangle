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
  static G4ThreeVector annihilation_x_axis;
  static G4ThreeVector annihilation_y_axis;
  static G4ThreeVector annihilation_z_axis;
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
        annihilation_x_axis = annihilation_y_axis.cross(annihilation_z_axis);
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
        << "\n  theta1: " << theta1
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
          const G4double originalTheta2 = std::acos(postStepPoint->GetMomentumDirection()*annihilation_z_axis);
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
          << "\n  original scattering plane2 (wrt photon1 axes): " << originalScatteringPlane2
          << "\n  originalTheta2 (wrt photon1 axes): " << originalTheta2
          << "\n  originalPhi2 (wrt photon1 axes): " << originalPhi2
          << G4endl;
#endif  // AnnihilationPhotonsSteppingActionPrinting

          // Calculate theta and phi of the Compton scatter of the second photon.
          // Scattering angle is unchanged.
          G4double theta2 = originalTheta2;
          // Draw azimuthal angle from the entangled distribution.
          G4double phi2 = originalPhi2;

          G4ThreeVector newMomentumDirectionPrime;  // in Compton scattering system
          newMomentumDirectionPrime.setRThetaPhi(1.,theta2,phi2);
          // Transform to global system
          const G4ThreeVector& v = newMomentumDirectionPrime;
          const G4ThreeVector& xp = annihilation_x_axis;
          const G4ThreeVector& yp = annihilation_y_axis;
          const G4ThreeVector& zp = annihilation_z_axis;
          G4ThreeVector newMomentum;
          newMomentum.setX(v.x()*xp.x()+v.y()*yp.x()+v.z()*zp.x());
          newMomentum.setY(v.x()*xp.y()+v.y()*yp.y()+v.z()*zp.y());
          newMomentum.setZ(v.x()*xp.z()+v.y()*yp.z()+v.z()*zp.z());
#if defined AnnihilationPhotonsSteppingActionPrinting || defined AnnihilationPhotonsSteppingActionConsistencyCheck
          const G4ThreeVector newScatteringPlane = newMomentum.cross(annihilation_z_axis).unit();
          const G4double newTheta2 = std::acos(newMomentum*annihilation_z_axis);
          const G4double newPhi2 = std::acos(newScatteringPlane*annihilation_y_axis);
#endif  // defined AnnihilationPhotonsSteppingActionPrinting || defined AnnihilationPhotonsSteppingActionConsistencyCheck
#ifdef AnnihilationPhotonsSteppingActionConsistencyCheck
          if (std::abs(newPhi2 - phi2) > 0.00001) {
            G4cout
            << "\n  Inconsistent calculation of phi"
#ifndef AnnihilationPhotonsSteppingActionPrinting
            << "\n  Original scattering plane2 (wrt photon1 axes): " << originalScatteringPlane2
            << "\n  New scattering plane2 (wrt photon1 axes): " << newScatteringPlane
            << "\n  originalTheta2 (wrt photon1 axes): " << originalTheta2
            << "\n  Desired theta2 (wrt photon1 axes): " << theta2
            << "\n  Achieved theta2 (wrt photon1 axes): " << newTheta2
            << "\n  originalPhi2 (wrt photon1 axes): " << originalPhi2
            << "\n  Desired phi2 (wrt photon1 axes): " << phi2
            << "\n  Achieved phi2 (wrt photon1 axes): " << newPhi2
#endif  // AnnihilationPhotonsSteppingActionPrinting
            << G4endl;
          }
#endif // AnnihilationPhotonsSteppingActionConsistencyCheck
#ifdef AnnihilationPhotonsSteppingActionPrinting
          G4cout
          << "\n  Original scattering plane2 (wrt photon1 axes): " << originalScatteringPlane2
          << "\n  New scattering plane2 (wrt photon1 axes): " << newScatteringPlane
          << "\n  originalTheta2 (wrt photon1 axes): " << originalTheta2
          << "\n  Desired theta2 (wrt photon1 axes): " << theta2
          << "\n  Achieved theta2 (wrt photon1 axes): " << newTheta2
          << "\n  originalPhi2 (wrt photon1 axes): " << originalPhi2
          << "\n  Desired phi2 (wrt photon1 axes): " << phi2
          << "\n  Achieved phi2 (wrt photon1 axes): " << newPhi2
          << G4endl;
#endif  // AnnihilationPhotonsSteppingActionPrinting
          track->SetMomentumDirection(newMomentum);


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
