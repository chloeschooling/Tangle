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
#include "G4Threading.hh"

AnnihilationPhotonsSteppingAction::AnnihilationPhotonsSteppingAction
(TangleRunAction* runAction)
: fpRunAction(runAction)
, fParentID1(0)
, fTheta1(0.)
, fPhi1(0.)
{}

void AnnihilationPhotonsSteppingAction::BeginOfEventAction()
{
  fComptonScatteringAnnihilationPhotonFound1 = false;
  fComptonScatteringAnnihilationPhotonFound2 = false;
}

void AnnihilationPhotonsSteppingAction::EndOfEventAction()
{
}

//#define AnnihilationPhotonsSteppingActionPrinting
#define AnnihilationPhotonsSteppingActionConsistencyCheck

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
        fParentID1 = track->GetParentID();
        fAnnihilation_z_axis = preStepPoint->GetMomentumDirection();
        fTheta1 = std::acos(postStepPoint->GetMomentumDirection()*fAnnihilation_z_axis);
        // Make y' perpendicular to global x-axis.
        fAnnihilation_y_axis = (fAnnihilation_z_axis.cross(G4ThreeVector(1,0,0))).unit();
        fAnnihilation_x_axis = fAnnihilation_y_axis.cross(fAnnihilation_z_axis);
        fScatteringPlane = postStepPoint->GetMomentum().cross(fAnnihilation_z_axis).unit();
        fPhi1 = std::acos(fScatteringPlane*fAnnihilation_y_axis);
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
        << "\n  Scattering plane: " << fScatteringPlane
        << "\n  fTheta1: " << fTheta1
        << "\n  fPhi1: " << fPhi1
        << G4endl;
#endif  // AnnihilationPhotonsSteppingActionPrinting
        data.fPhi1 = fPhi1;
      }
    } else {
      // First photon found so look for second
      if (!fComptonScatteringAnnihilationPhotonFound2) {
        if (track->GetCurrentStepNumber() == 1) {
          fComptonScatteringAnnihilationPhotonFound2 = true;
          // This is the first step of the second photon
#ifdef AnnihilationPhotonsSteppingActionConsistencyCheck
          if (track->GetParentID() != fParentID1)
          {
            G4cout <<
            "\n  Annihilation photons do not have the same parent ID"
            << G4endl;
            fComptonScatteringAnnihilationPhotonFound2 = false;
            return;
          }
          const G4double dotProduct = preStepPoint->GetMomentumDirection().unit() * fAnnihilation_z_axis;
          if (dotProduct > -0.999999) {
            G4cout <<
            "\n  Annihilation photons not in opposite directions: dot product" << dotProduct
            << G4endl;
            fComptonScatteringAnnihilationPhotonFound2 = false;
            return;
          }
#endif // AnnihilationPhotonsSteppingActionConsistencyCheck
#if defined AnnihilationPhotonsSteppingActionPrinting || defined AnnihilationPhotonsSteppingActionConsistencyCheck
          const G4ThreeVector originalScatteringPlane2 = postStepPoint->GetMomentum().cross(fAnnihilation_z_axis).unit();
          // Theta is wrt photon2 direction - hence minus signs
          const G4double originalTheta2 = std::acos(-postStepPoint->GetMomentumDirection()*fAnnihilation_z_axis);
          const G4double originalPhi2 = std::acos(originalScatteringPlane2*fAnnihilation_y_axis);
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
          << "\n  originalScatteringPlane2: " << originalScatteringPlane2
          << "\n  originalTheta2: " << originalTheta2
          << "\n  originalPhi2: " << originalPhi2
          << G4endl;
#endif  // AnnihilationPhotonsSteppingActionPrinting

          // Calculate theta and phi of the Compton scatter of the second photon.
          // Scattering angle is unchanged.
          G4double theta2 = originalTheta2;
          // Draw azimuthal angle from the entangled distribution.
          G4double phi2 = originalPhi2;

          G4ThreeVector newMomentumDirectionPrime;
          // In photon1 system - hence the pi-theta2
          newMomentumDirectionPrime.setRThetaPhi(1.,pi-theta2,phi2);
          // Transform to global system
          const G4ThreeVector& v = newMomentumDirectionPrime;
          const G4ThreeVector& xp = fAnnihilation_x_axis;
          const G4ThreeVector& yp = fAnnihilation_y_axis;
          const G4ThreeVector& zp = fAnnihilation_z_axis;
          G4ThreeVector newMomentumDirection;
          newMomentumDirection.setX(v.x()*xp.x()+v.y()*yp.x()+v.z()*zp.x());
          newMomentumDirection.setY(v.x()*xp.y()+v.y()*yp.y()+v.z()*zp.y());
          newMomentumDirection.setZ(v.x()*xp.z()+v.y()*yp.z()+v.z()*zp.z());
#if defined AnnihilationPhotonsSteppingActionPrinting || defined AnnihilationPhotonsSteppingActionConsistencyCheck
          const G4ThreeVector newScatteringPlane = newMomentumDirection.cross(-fAnnihilation_z_axis).unit();
          const G4double newTheta2 = std::acos(-newMomentumDirection*fAnnihilation_z_axis);
          const G4double newPhi2 = std::acos(newScatteringPlane*fAnnihilation_y_axis);
#endif  // defined AnnihilationPhotonsSteppingActionPrinting || defined AnnihilationPhotonsSteppingActionConsistencyCheck
#ifdef AnnihilationPhotonsSteppingActionConsistencyCheck
          if (std::abs(newPhi2 - phi2) > 0.00001 || std::abs(newTheta2 - theta2) > 0.00001) {
            G4cout
            << "\n  Inconsistent calculation of phi"
#ifndef AnnihilationPhotonsSteppingActionPrinting
            << "\n  Original scattering plane2: " << originalScatteringPlane2
            << "\n  New scattering plane2: " << newScatteringPlane
            << "\n  originalTheta2: " << originalTheta2
            << "\n  Desired theta2: " << theta2
            << "\n  Achieved theta2: " << newTheta2
            << "\n  originalPhi2: " << originalPhi2
            << "\n  Desired phi2: " << phi2
            << "\n  Achieved phi2: " << newPhi2
#endif  // AnnihilationPhotonsSteppingActionPrinting
            << G4endl;
          }
#endif // AnnihilationPhotonsSteppingActionConsistencyCheck
#ifdef AnnihilationPhotonsSteppingActionPrinting
          G4cout
          << "\n  Original scattering plane2: " << originalScatteringPlane2
          << "\n  New scattering plane2: " << newScatteringPlane
          << "\n  originalTheta2: " << originalTheta2
          << "\n  Desired theta2: " << theta2
          << "\n  Achieved theta2: " << newTheta2
          << "\n  originalPhi2: " << originalPhi2
          << "\n  Desired phi2: " << phi2
          << "\n  Achieved phi2: " << newPhi2
          << G4endl;
#endif  // AnnihilationPhotonsSteppingActionPrinting
          track->SetMomentumDirection(newMomentumDirection);


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
