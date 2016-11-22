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
//  if (fComptonScatteringAnnihilationPhotonFound1 &&
//      fComptonScatteringAnnihilationPhotonFound2) {
//    const G4ThreeVector annihilation_z_axis = (fComptonScatteredPhotonPosition1 - fPhotonOriginPosition1).unit();
//    G4ThreeVector difference = annihilation_z_axis - (fPhotonOriginPosition2 - fComptonScatteredPhotonPosition2).unit();
//    if (difference.mag() > 0.0000001) {
//      G4cerr << "\n  Axis mis-alignment" << G4endl;
//      abort();
//    }
//    G4double polarisationScalarProduct = fPhotonPolarisation1 * fPhotonPolarisation2;
////    G4cout << "polarisationScalarProduct: " << polarisationScalarProduct << G4endl;
//    if (std::abs(polarisationScalarProduct) > 0.00001) {
//      G4cout << "\n  Polarisations not at right angles; scalar product: " << polarisationScalarProduct << G4endl;
//    }
//    G4ThreeVector polarisationVectorProduct = fPhotonPolarisation1.cross(fPhotonPolarisation2);
////    G4cout << "polarisationVectorProduct: " << polarisationVectorProduct << G4endl;
////    G4cout << "annihilation_z_axis: " << annihilation_z_axis << G4endl;
//    if (std::abs((polarisationVectorProduct.cross(annihilation_z_axis).mag())) > 0.00001) {
//      G4cout << "\n  Polarisations not perpendicular to direction;"
//      << "\n  vector product: " << polarisationVectorProduct
//      << "\n  direction: " << annihilation_z_axis
//      << G4endl;
//    }
//    // Make y' perpendicular to global x-axis.
//    const G4ThreeVector annihilation_y_axis = (annihilation_z_axis.cross(G4ThreeVector(1,0,0))).unit();
////    const G4ThreeVector annihilation_x_axis = annihilation_y_axis.cross(annihilation_z_axis);
//    const G4ThreeVector v1 = fComptonScatteredPhotonMomentum1.cross(annihilation_z_axis);
//    G4cout << "\n  Scattering plane1: " << v1.unit() << G4endl;
//    const G4double& v1_mag = v1.mag();
//    G4double phi1 = std::acos(v1*annihilation_y_axis/v1_mag);
//    G4cout << "\n  phi1: " << phi1 << G4endl;
//    const G4ThreeVector v2 = fComptonScatteredPhotonMomentum2.cross(annihilation_z_axis);
//    G4cout << "\n  Scattering plane2: " << v2.unit() << G4endl;
//    const G4double& v2_mag = v2.mag();
//    G4double phi2 = std::acos(v2*annihilation_y_axis/v2_mag);
//    G4cout << "\n  phi2: " << phi2 << G4endl;
//    G4cout << "\n  phi2-phi1: " << phi2-phi1 << G4endl;
//    G4cout << "\n  data: " << data.fPhi1 << ", " << data.fPhi2 << G4endl;
//    fpRunAction->RecordData(TangleRunAction::Data(phi2,phi1));
//  } else {
////    G4cout << "Compton scattering of two photons not found in this event." << G4endl;
//  }
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
        G4cout
        << "\n  1st photon found: trackID: " << track->GetTrackID()
        << "\n  preStepPointPosition: " << preStepPoint->GetPosition()
        << "\n  postStepPointPosition: " << postStepPoint->GetPosition()
        << "\n  preStepPointMomentum: " << preStepPoint->GetMomentum()
        << "\n  postStepPointMomentum: " << postStepPoint->GetMomentum()
        << "\n  preStepPolarisation: " << preStepPoint->GetPolarization()
        << "\n  postStepPolarisation: " << postStepPoint->GetPolarization()
        << G4endl;
        G4cout << "\n  Scattering plane: "
        << preStepPoint->GetMomentum().cross(postStepPoint->GetMomentum()).unit()
        << G4endl;
//        fPhotonOriginPosition1 = preStepPoint->GetPosition();
//        fPhotonPolarisation1 = preStepPoint->GetPolarization();
//        // We want the scattered photon
//        fComptonScatteredPhotonPosition1 = postStepPoint->GetPosition();
//        fComptonScatteredPhotonMomentum1 = postStepPoint->GetMomentum();
//        G4cout
//        << "\n  Scattered photon position: " << fComptonScatteredPhotonPosition1
//        << "\n  momentum: " << fComptonScatteredPhotonMomentum1
//        << G4endl;
        const G4ThreeVector annihilation_z_axis = preStepPoint->GetMomentumDirection();
        // Make y' perpendicular to global x-axis.
        const G4ThreeVector annihilation_y_axis = (annihilation_z_axis.cross(G4ThreeVector(1,0,0))).unit();
//        const G4ThreeVector annihilation_x_axis = annihilation_y_axis.cross(annihilation_z_axis);
        const G4ThreeVector scatteringPlane = postStepPoint->GetMomentum().cross(annihilation_z_axis).unit();
        G4cout << "\n  Scattering plane1: " << scatteringPlane << G4endl;
        G4double phi1 = std::acos(scatteringPlane*annihilation_y_axis);
        G4cout << "\n  phi1: " << phi1 << G4endl;
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
          }
          G4cout
          << "\n  2nd photon found: trackID: " << track->GetTrackID()
          << "\n  preStepPointPosition: " << preStepPoint->GetPosition()
          << "\n  postStepPointPosition: " << postStepPoint->GetPosition()
          << "\n  preStepPointMomentum: " << preStepPoint->GetMomentum()
          << "\n  postStepPointMomentum: " << postStepPoint->GetMomentum()
          << "\n  preStepPolarisation: " << preStepPoint->GetPolarization()
          << "\n  postStepPolarisation: " << postStepPoint->GetPolarization()
          << G4endl;
          G4cout << "\n  Scattering plane: "
          << preStepPoint->GetMomentum().cross(postStepPoint->GetMomentum()).unit()
          << G4endl;
//          fPhotonOriginPosition2 = preStepPoint->GetPosition();
//          fPhotonPolarisation2 = preStepPoint->GetPolarization();
//          // We want the scattered photon
//          fComptonScatteredPhotonPosition2 = postStepPoint->GetPosition();
//          fComptonScatteredPhotonMomentum2 = postStepPoint->GetMomentum();
//          G4cout
//          << "\n  Scattered photon position: " << fComptonScatteredPhotonPosition2
//          << "\n  momentum: " << fComptonScatteredPhotonMomentum2
//          << G4endl;
          const G4ThreeVector annihilation_z_axis = preStepPoint->GetMomentumDirection();
          // Make y' perpendicular to global x-axis.
          const G4ThreeVector annihilation_y_axis = (annihilation_z_axis.cross(G4ThreeVector(1,0,0))).unit();
          //        const G4ThreeVector annihilation_x_axis = annihilation_y_axis.cross(annihilation_z_axis);
          const G4ThreeVector scatteringPlane = postStepPoint->GetMomentum().cross(annihilation_z_axis).unit();
          G4cout << "\n  Scattering plane2: " << scatteringPlane << G4endl;
          G4double phi2 = std::acos(scatteringPlane*annihilation_y_axis);
          G4cout << "\n  phi2: " << phi2 << G4endl;
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
