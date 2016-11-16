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
  if (fComptonScatteringAnnihilationPhotonFound1 &&
      fComptonScatteringAnnihilationPhotonFound2) {
    const G4ThreeVector annihilation_z_axis = (fComptonScatteredPhotonPosition1 - fPhotonOriginPosition1).unit();
    G4ThreeVector difference = annihilation_z_axis - (fPhotonOriginPosition2 - fComptonScatteredPhotonPosition2).unit();
    if (difference.mag() > 0.0000001) {
      G4cerr << "Axis mis-alignment" << G4endl;
      abort();
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
      G4cout << "Polarisations not perpendicular to direction;"
      << "\n  vector product: " << polarisationVectorProduct
      << "\n  direction: " << annihilation_z_axis
      << G4endl;
    }
    // Make y' perp. to polarisation of first photon
    const G4ThreeVector annihilation_y_axis = (annihilation_z_axis.cross(fPhotonPolarisation1)).unit();
    const G4ThreeVector annihilation_x_axis = annihilation_y_axis.cross(annihilation_z_axis);
    const G4ThreeVector v1 = fComptonScatteredPhotonMomentum1.cross(annihilation_z_axis);
    G4cout << "Scattering plane1: " << v1.unit() << G4endl;
    const G4double& v1_mag = v1.mag();
    G4double phi1 = std::acos(v1*annihilation_y_axis/v1_mag);
    G4cout << "phi1: " << phi1 << G4endl;
    const G4ThreeVector v2 = fComptonScatteredPhotonMomentum2.cross(annihilation_z_axis);
    G4cout << "Scattering plane2: " << v2.unit() << G4endl;
    const G4double& v2_mag = v2.mag();
    G4double phi2 = std::acos(v2*annihilation_y_axis/v2_mag);
    G4cout << "phi2: " << phi2 << G4endl;
    G4cout << "phi2-phi1: " << phi2-phi1 << G4endl;
    fpRunAction->RecordData(TangleRunAction::Data(phi2,phi1));
  } else {
    G4cout << "Compton scattering of two photons not found in this event." << G4endl;
  }
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
        G4cout
        << "1st photon found: trackID: " << track->GetTrackID()
        << ", preStepPointPosition: " << preStepPoint->GetPosition()
        << ", postStepPointPosition: " << postStepPoint->GetPosition()
        << ", preStepPointMomentum: " << preStepPoint->GetMomentum()
        << ", postStepPointMomentum: " << postStepPoint->GetMomentum()
        << ", preStepPolarisation: " << preStepPoint->GetPolarization()
        << ", postStepPolarisation: " << postStepPoint->GetPolarization()
        << G4endl;
        G4cout << "Scattering plane: "
        << preStepPoint->GetMomentum().cross(postStepPoint->GetMomentum()).unit()
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
          << ", preStepPolarisation: " << preStepPoint->GetPolarization()
          << ", postStepPolarisation: " << postStepPoint->GetPolarization()
          << G4endl;
          G4cout << "Scattering plane: "
          << preStepPoint->GetMomentum().cross(postStepPoint->GetMomentum()).unit()
          << G4endl;
          fPhotonOriginPosition2 = preStepPoint->GetPosition();
          fPhotonPolarisation2 = preStepPoint->GetPolarization();
          // We want the scattered photon
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

  return;
}