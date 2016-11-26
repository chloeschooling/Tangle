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
#include "G4Gamma.hh"
#include "G4Threading.hh"

AnnihilationPhotonsSteppingAction::AnnihilationPhotonsSteppingAction
(TangleRunAction* runAction)
: fpRunAction(runAction)
, fTrackID1(0)
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

namespace {
  void CalculateThetaPhi
  (const G4ThreeVector& v,
   const G4ThreeVector& z_axis,
   // Output quantities
   G4ThreeVector& y_axis,
   G4ThreeVector& x_axis,
   G4double& cosTheta,
   G4double& theta,
   G4double& phi)
  {
    cosTheta = v*z_axis;
    theta = std::acos(cosTheta);
    // Make y' perpendicular to global x-axis.
    y_axis = (z_axis.cross(G4ThreeVector(1,0,0))).unit();
    x_axis = y_axis.cross(z_axis);
    const G4ThreeVector ontoXYPlane = v.cross(z_axis);
    // ontoXYPlane is a vector in the xy-plane, but perpendicular to the
    // projection of the scattered photon, so
    const G4double projection_x = -ontoXYPlane*y_axis;
    const G4double projection_y = ontoXYPlane*x_axis;
    phi = std::atan2(projection_y,projection_x);
  }
}

void AnnihilationPhotonsSteppingAction::UserSteppingAction(const G4Step* step)
{
  G4Track* track = step->GetTrack();

  if (track->GetDefinition() != G4Gamma::Gamma()) return;

  if (track->GetCurrentStepNumber() != 1) return;

  const G4VProcess* creatorProcess = track->GetCreatorProcess();
  if (creatorProcess == nullptr) return;
  if (creatorProcess->GetProcessName() != "annihil") return;

  G4StepPoint* preStepPoint = step->GetPreStepPoint();

  G4StepPoint* postStepPoint = step->GetPostStepPoint();
  const G4VProcess* postProcessDefinedStep = postStepPoint->GetProcessDefinedStep();
  if (postProcessDefinedStep == nullptr) return;
  if (postProcessDefinedStep->GetProcessName() != "compt") return;

  // This is the frst step of an annihilation photon that Compton scatters.

  if (!fComptonScatteringAnnihilationPhotonFound1) {

    fComptonScatteringAnnihilationPhotonFound1 = true;

    fTrackID1 = track->GetTrackID();
    fParentID1 = track->GetParentID();

    fPhoton1_z_axis = preStepPoint->GetMomentumDirection();

    G4ThreeVector photon1_y_axis;
    G4ThreeVector photon1_x_axis;
    CalculateThetaPhi
    (postStepPoint->GetMomentumDirection(),
     fPhoton1_z_axis,
     photon1_y_axis,
     photon1_x_axis,
     fCosTheta1,
     fTheta1,
     fPhi1);

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
    << "\n  fTheta1: " << fTheta1
    << "\n  fPhi1: " << fPhi1
    << G4endl;
#endif  // AnnihilationPhotonsSteppingActionPrinting

    data.fPhi1 = fPhi1;

  } else if (!fComptonScatteringAnnihilationPhotonFound2) {

    // Second photon found
    fComptonScatteringAnnihilationPhotonFound2 = true;

    const G4ThreeVector photon2_z_axis = preStepPoint->GetMomentumDirection();
    G4ThreeVector photon2_y_axis;
    G4ThreeVector photon2_x_axis;
    G4double originalCosTheta2;
    G4double originalTheta2;
    G4double originalPhi2;
    CalculateThetaPhi
    (postStepPoint->GetMomentumDirection(),
     photon2_z_axis,
     photon2_y_axis,
     photon2_x_axis,
     originalCosTheta2,
     originalTheta2,
     originalPhi2);

#ifdef AnnihilationPhotonsSteppingActionConsistencyCheck
    // If there are more than one annhilations in an event and one of the photons
    // of the first does not Compton scatter then we may pick up an annhilation
    // photon from the second, so we must do these checks.  If they fail then
    // there's a VERY SMALL chance that we will miss an annhilation.
    if (track->GetParentID() != fParentID1)
    {
      G4cout
      << "\n  Annihilation photons do not have the same parent ID"
      << "\n  track/parent IDs: " << fTrackID1 << '/' << fParentID1
      << ',' << track->GetTrackID() << '/' << track->GetParentID()
      << G4endl;
      // We are out of step - no easy way to get back in step with this algorithm
      // Do not allow any more annihilations this event.
      return;
    }
    const G4double dotProduct = preStepPoint->GetMomentumDirection().unit()*fPhoton1_z_axis;
    if (dotProduct > -0.999999) {
      G4cout <<
      "\n  Annihilation photons not in opposite directions: dot product" << dotProduct
      << G4endl;
      // We are out of step - no easy way to get back in step with this algorithm
      // Do not allow any more annihilations this event.
      return;
    }
#endif // AnnihilationPhotonsSteppingActionConsistencyCheck

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
    << "\n  originalTheta2: " << originalTheta2
    << "\n  originalPhi2: " << originalPhi2
    << G4endl;
#endif  // AnnihilationPhotonsSteppingActionPrinting

    // Calculate theta and phi of the Compton scatter of the second photon.
    // Scattering angle is unchanged.
    G4double desiredTheta2 = originalTheta2;
    // Draw azimuthal angle from the entangled distribution.  <<<<<<<<<<<<< NEXT JOB
    G4double desiredPhi2 = originalPhi2;
    if (desiredPhi2 > pi) {
      desiredPhi2 -= twopi;
    }
    if (desiredPhi2 < -pi) {
      desiredPhi2 += twopi;
    }

    G4ThreeVector newMomentumDirectionPrime;
    // In frame of second photon (denoted by "prime")
    newMomentumDirectionPrime.setRThetaPhi(1.,desiredTheta2,desiredPhi2);
    // Transform to global system
    // Some aliases
    const G4ThreeVector& v = newMomentumDirectionPrime;
    const G4ThreeVector& xp = photon2_x_axis;
    const G4ThreeVector& yp = photon2_y_axis;
    const G4ThreeVector& zp = photon2_z_axis;
    // In global system
    G4ThreeVector newMomentumDirection;
    newMomentumDirection.setX(v.x()*xp.x()+v.y()*yp.x()+v.z()*zp.x());
    newMomentumDirection.setY(v.x()*xp.y()+v.y()*yp.y()+v.z()*zp.y());
    newMomentumDirection.setZ(v.x()*xp.z()+v.y()*yp.z()+v.z()*zp.z());

#if defined AnnihilationPhotonsSteppingActionPrinting || defined AnnihilationPhotonsSteppingActionConsistencyCheck
    G4double newCosTheta2;
    G4double newTheta2;
    G4double newPhi2;
    CalculateThetaPhi
    (newMomentumDirection,
     photon2_z_axis,
     photon2_y_axis,
     photon2_x_axis,
     newCosTheta2,
     newTheta2,
     newPhi2);
#endif  // defined AnnihilationPhotonsSteppingActionPrinting || defined AnnihilationPhotonsSteppingActionConsistencyCheck

#ifdef AnnihilationPhotonsSteppingActionConsistencyCheck
    if (std::abs(newPhi2 - desiredPhi2) > 0.00001 || std::abs(newTheta2 - desiredTheta2) > 0.00001) {
      G4cout
      << "\n  Inconsistent calculation of phi"
      << "\n  originalTheta2: " << originalTheta2
      << "\n  desiredTheta2: " << desiredTheta2
      << "\n  newTheta2: " << newTheta2
      << "\n  originalPhi2: " << originalPhi2
      << "\n  desiredPhi2: " << desiredPhi2
      << "\n  newPhi2: " << newPhi2
      << G4endl;
    }
#endif // AnnihilationPhotonsSteppingActionConsistencyCheck

#ifdef AnnihilationPhotonsSteppingActionPrinting
    G4cout
    << "\n  originalTheta2: " << originalTheta2
    << "\n  desiredTheta2: " << desiredTheta2
    << "\n  newTheta2: " << newTheta2
    << "\n  originalPhi2: " << originalPhi2
    << "\n  desiredPhi2: " << desiredPhi2
    << "\n  newPhi2: " << newPhi2
    << G4endl;
#endif  // AnnihilationPhotonsSteppingActionPrinting

    track->SetMomentumDirection(newMomentumDirection);


    data.fPhi2 = newPhi2;
    fpRunAction->RecordData(data);

    //Reset for further possible annihilations in this event.
    fComptonScatteringAnnihilationPhotonFound1 = false;
    fComptonScatteringAnnihilationPhotonFound2 = false;
  }

  return;
}
