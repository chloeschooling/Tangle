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
  fComptonScatteringAnnihilationPhotonFound1 = false;
  fComptonScatteringAnnihilationPhotonFound2 = false;
}

void TangleSensitiveDetector::EndOfEvent(G4HCofThisEvent*)
{
  if (fComptonScatteringAnnihilationPhotonFound1 &&
      fComptonScatteringAnnihilationPhotonFound2) {
    const G4ThreeVector annihilation_z_axis = (fComptonScatteredPhotonPosition1 - fPhotonOriginPosition1).unit();
    G4ThreeVector difference = annihilation_z_axis - (fComptonScatteredPhotonPosition2 - fPhotonOriginPosition2).unit();
    if (difference.mag() > 0.0000001) {
      G4cerr << "Axis mis-alignment" << G4endl;
//      abort();
    }
    G4double polarisationScalarProduct = fPhotonPolarisation1 * fPhotonPolarisation2;
    G4cout << "polarisationScalarProduct: " << polarisationScalarProduct << G4endl;
    if (std::abs(polarisationScalarProduct) > 0.00001) {
      G4cout << "Polarisations not at right angles; scalar product: " << polarisationScalarProduct << G4endl;
    }
    G4ThreeVector polarisationVectorProduct = fPhotonPolarisation1.cross(fPhotonPolarisation2);
    G4cout << "polarisationVectorProduct: " << polarisationVectorProduct << G4endl;
    G4cout << "annihilation_z_axis: " << annihilation_z_axis << G4endl;
    if (std::abs((polarisationVectorProduct.cross(annihilation_z_axis).mag())) > 0.00001) {
      G4cout << "Polarisations not parallel to direction;"
      << "\n  vector product: " << polarisationVectorProduct
      << "\n  direction: " << annihilation_z_axis
      << G4endl;
    }
//    const G4ThreeVector annihilation_y_axis = (annihilation_z_axis.cross(G4ThreeVector(1.,0.,0.))).unit();
    // Make y' perp. to polarisation of first photon
    const G4ThreeVector annihilation_y_axis = (annihilation_z_axis.cross(fPhotonPolarisation1)).unit();
    const G4ThreeVector annihilation_x_axis = annihilation_y_axis.cross(annihilation_z_axis);
    const G4ThreeVector v1 = fComptonScatteredPhotonMomentum1.cross(annihilation_z_axis);
    const G4double& v1_mag = v1.mag();
    G4double phi1 = std::acos(v1*annihilation_y_axis/v1_mag);
    G4cout << "phi1: " << phi1 << G4endl;
    const G4ThreeVector v2 = fComptonScatteredPhotonMomentum2.cross(annihilation_z_axis);
    const G4double& v2_mag = v2.mag();
    G4double phi2 = std::acos(v2*annihilation_y_axis/v2_mag);
    G4cout << "phi2: " << phi2 << G4endl;
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
    if (!fComptonScatteringAnnihilationPhotonFound1) {
      if (track->GetCurrentStepNumber() == 1) {
        fComptonScatteringAnnihilationPhotonFound1 = true;
        // This is the first step of the first photon
        G4cout
        << "1st photon found: trackID: " << track->GetTrackID()
        << ", preStepPointPosition: " << preStepPoint->GetPosition()
        << ", postStepPointPosition: " << postStepPoint->GetPosition()
        << ", preStepPointMomentum: " << preStepPoint->GetMomentum()
        << ", postStepPointMomentum: " << postStepPoint->GetMomentum()
        << ", polarisation: " << preStepPoint->GetPolarization()
        << G4endl;
        fPhotonOriginPosition1 = preStepPoint->GetPosition();
        fPhotonPolarisation1 = preStepPoint->GetPolarization();
        // We want the scattered photon
        fComptonScatteredPhotonPosition1 = postStepPoint->GetPosition();
        fComptonScatteredPhotonMomentum1 = postStepPoint->GetMomentum();
        G4cout
        << "Scattered photon position: " << fComptonScatteredPhotonPosition1
        << ", momentum: " << fComptonScatteredPhotonMomentum1
        << G4endl;
      }
    } else {
      // First photon found so look for second
      if (!fComptonScatteringAnnihilationPhotonFound2) {
        if (track->GetCurrentStepNumber() == 1) {
          fComptonScatteringAnnihilationPhotonFound2 = true;
          // This is the first step of the second photon
          G4cout
          << "2nd photon found: trackID: " << track->GetTrackID()
          << ", preStepPointPosition: " << preStepPoint->GetPosition()
          << ", postStepPointPosition: " << postStepPoint->GetPosition()
          << ", preStepPointMomentum: " << preStepPoint->GetMomentum()
          << ", postStepPointMomentum: " << postStepPoint->GetMomentum()
          << ", polarisation: " << preStepPoint->GetPolarization()
          << G4endl;
          fPhotonOriginPosition2 = preStepPoint->GetPosition();
          fPhotonPolarisation2 = preStepPoint->GetPolarization();
          fComptonScatteredPhotonPosition2 = postStepPoint->GetPosition();
          fComptonScatteredPhotonMomentum2 = postStepPoint->GetMomentum();
          G4cout
          << "Scattered photon position: " << fComptonScatteredPhotonPosition2
          << ", momentum: " << fComptonScatteredPhotonMomentum2
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
