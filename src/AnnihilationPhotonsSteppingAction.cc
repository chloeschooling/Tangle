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
, fAnnihilationPhotonFound1(false)
, fComptonScatteringAnnihilationPhotonFound1(false)
, fTrackID1(0)
, fParentID1(0)
, fTheta1(0.)
, fPhi1(0.)
{}

void AnnihilationPhotonsSteppingAction::BeginOfEventAction()
{
  fAnnihilationPhotonFound1 = false;
  fComptonScatteringAnnihilationPhotonFound1 = false;
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

  // This is the frst step of an annihilation photon.

  ///////////////////////////////////////////////////////////////////////////
  if (!fAnnihilationPhotonFound1) {

    fAnnihilationPhotonFound1 = true;

    if (postProcessDefinedStep->GetProcessName() != "compt") return;

    fComptonScatteringAnnihilationPhotonFound1 = true;

    fTrackID1 = track->GetTrackID();
    fParentID1 = track->GetParentID();

    fPhoton1_z_axis = preStepPoint->GetMomentumDirection();

    G4ThreeVector photon1_y_axis;  // dummy, i.e., not used.
    G4ThreeVector photon1_x_axis;  // dummy, i.e., not used.
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

    data.fTheta1 = fTheta1;
    data.fPhi1 = fPhi1;

    ///////////////////////////////////////////////////////////////////////////
  } else {
    ///////////////////////////////////////////////////////////////////////////

    // Second photon found

    // Unless BOTH annihilation photons undergo Compton scattering, do nothing.
    G4bool doNothing = false;
    if (!fComptonScatteringAnnihilationPhotonFound1) {
      doNothing = true;
#ifdef AnnihilationPhotonsSteppingActionPrinting
      G4cout << "First annihilation photon did not undergo Compton scattering." << G4endl;
#endif  // AnnihilationPhotonsSteppingActionPrinting
    }
    if (postProcessDefinedStep->GetProcessName() != "compt") {
      doNothing = true;
#ifdef AnnihilationPhotonsSteppingActionPrinting
      G4cout << "Second annihilation photon did not undergo Compton scattering." << G4endl;
#endif  // AnnihilationPhotonsSteppingActionPrinting
    }
    if (doNothing) {
      //Reset for further possible annihilations in this event.
      fAnnihilationPhotonFound1 = false;
      fComptonScatteringAnnihilationPhotonFound1 = false;
      return;
    }

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
    if (track->GetParentID() != fParentID1)
    {
      G4cout
      << "\n  Annihilation photons do not have the same parent ID"
      << "\n  track/parent IDs: " << fTrackID1 << '/' << fParentID1
      << ',' << track->GetTrackID() << '/' << track->GetParentID()
      << G4endl;
      //Reset for further possible annihilations in this event.
      fAnnihilationPhotonFound1 = false;
      fComptonScatteringAnnihilationPhotonFound1 = false;
      return;
    }
    const G4double dotProduct = preStepPoint->GetMomentumDirection().unit()*fPhoton1_z_axis;
    if (dotProduct > -0.999999) {
      G4cout <<
      "\n  Annihilation photons not in opposite directions: dot product" << dotProduct
      << G4endl;
      //Reset for further possible annihilations in this event.
      fAnnihilationPhotonFound1 = false;
      fComptonScatteringAnnihilationPhotonFound1 = false;
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
    // Draw the azimuthal angle from the entangled distribution:
    // A + B * cos(2*deltaPhi), or rather C + D * cos(2*deltaPhi), where
    // C = A / (A + |B|) and D = B / (A + |B|), so that maximum is 1.
    const G4double sin2Theta1 = 1.-fCosTheta1*fCosTheta1;
    const G4double sin2Theta2 = 1.-originalCosTheta2*originalCosTheta2;
    // Pryce and Ward
    const G4double A =
    ((std::pow(1.-fCosTheta1,3.))+2.)*(std::pow(1.-originalCosTheta2,3.)+2.)/
    ((std::pow(2.-fCosTheta1,3.)*std::pow(2.-originalCosTheta2,3.)));
    const G4double B = -(sin2Theta1*sin2Theta2)/
    ((std::pow(2.-fCosTheta1,2.)*std::pow(2.-originalCosTheta2,2.)));
    const G4double C = A / (A + std::abs(B));
    const G4double D = B / (A + std::abs(B));
//    G4cout << "A,B,C,D: " << A << ',' << B  << ',' << C << ',' << D << G4endl;
//    // Snyder et al
//    const G4double& k0 = preStepPoint->GetKineticEnergy();
//    const G4double k1 = k0/(2.-fCosTheta1);
//    const G4double k2 = k0/(2.-originalCosTheta2);
//    const G4double gamma1 = k1/k0+k0/k1;
//    const G4double gamma2 = k2/k0+k0/k2;
//    const G4double A1 = gamma1*gamma2-gamma1*sin2Theta2-gamma2*sin2Theta1;
//    const G4double B1 = 2.*sin2Theta1*sin2Theta2;
//    // That's A1 + B1*sin2(deltaPhi) = A1 + B1*(0.5*(1.-cos(2.*deltaPhi).
//    const G4double ASnyder = A1 + 0.5*B1;
//    const G4double BSnyder = -0.5*B1;
//    const G4double CSnyder = ASnyder / (ASnyder + std::abs(BSnyder));
//    const G4double DSnyder = BSnyder / (ASnyder + std::abs(BSnyder));
//    G4cout << "A,B,C,D(Snyder): " << ASnyder << ',' << BSnyder << ',' << CSnyder << ',' << DSnyder << G4endl;

    // Sample delta phi
    G4double deltaPhi;
    const G4int maxCount = 999999;
    G4int iCount = 0;
    for (; iCount < maxCount; ++iCount) {
      deltaPhi = twopi * G4UniformRand();
      if (G4UniformRand() < C + D * cos(2.*deltaPhi)) break;
    }
    if (iCount >= maxCount ) {
      G4cout << "Random delta phi not found in " << maxCount << " tries - carry on anyway." << G4endl;
    }

    // Thus, the desired second photon azimuth
    G4double desiredPhi2 = deltaPhi - fPhi1;
    // Minus sign in above statement because of opposite coordinate system orientations.
    if (desiredPhi2 > pi) {
      desiredPhi2 -= twopi;
    }
    if (desiredPhi2 < -pi) {
      desiredPhi2 += twopi;
    }

    // Scattering angle is unchanged.
    const G4double desiredTheta2 = originalTheta2;

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
    // And don't forget to do the same for the Compton electron.  <<<<<<<<< NEXT JOB

    data.fTheta2 = newTheta2;
    data.fPhi2 = newPhi2;
    fpRunAction->RecordData(data);

    //Reset for further possible annihilations in this event.
    fAnnihilationPhotonFound1 = false;
    fComptonScatteringAnnihilationPhotonFound1 = false;
  }
  ///////////////////////////////////////////////////////////////////////////

  return;
}
