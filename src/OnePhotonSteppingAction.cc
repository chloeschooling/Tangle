#include "OnePhotonSteppingAction.hh"

#include "TangleRunAction.hh"

#include "G4Step.hh"
#include "G4VProcess.hh"

OnePhotonSteppingAction::OnePhotonSteppingAction(TangleRunAction* runAction)
: fpRunAction(runAction)
, fComptonScatteringFound(false)
{}

void OnePhotonSteppingAction::ResetAtBeginOfEvent()
{
  fComptonScatteringFound = false;
}

void OnePhotonSteppingAction::UserSteppingAction(const G4Step* step)
{
  // Debugging a single photon, e.g:
  //  /gps/particle gamma
  //  /gps/energy 500 keV
  //  /gps/direction 0 0 1
  //  /gps/polarization 0 1 0
  //  /gps/position 0 0 0 m

  G4StepPoint* postStepPoint = step->GetPostStepPoint();
  const G4VProcess* postProcessDefinedStep = postStepPoint->GetProcessDefinedStep();
  if (postProcessDefinedStep == nullptr) return;

  if (postProcessDefinedStep->GetProcessName() == "compt") {

    // This is a Compton scatter.
    if (!fComptonScatteringFound) {
      G4Track* track = step->GetTrack();
      if (track->GetCurrentStepNumber() == 1) {
        fComptonScatteringFound = true;

        // This is the first Compton scatter
        G4StepPoint* preStepPoint = step->GetPreStepPoint();
        
        G4cout
        << "\nCompton scatter found: trackID: " << track->GetTrackID()
        << "\npreStepPointPosition: " << preStepPoint->GetPosition()
        << "\npostStepPointPosition: " << postStepPoint->GetPosition()
        << "\npreStepPointMomentum: " << preStepPoint->GetMomentum()
        << "\npostStepPointMomentum: " << postStepPoint->GetMomentum()
        << "\npreStepPolarisation: " << preStepPoint->GetPolarization()
        << "\npostStepPolarisation: " << postStepPoint->GetPolarization()
        << G4endl;
        G4cout << "\nScattering plane: "
        << preStepPoint->GetMomentum().cross(postStepPoint->GetMomentum()).unit()
        << G4endl;

        const G4ThreeVector& photon_z_axis = preStepPoint->GetMomentumDirection();
        G4ThreeVector photon_y_axis = photon_z_axis.cross(G4ThreeVector(1,0,0));
        // If direction is very close to x-axis...
        if (photon_y_axis.mag() < 1e-9) {
          photon_y_axis = G4ThreeVector(0,1,0);
        } else {
          photon_y_axis = photon_y_axis.unit();
        }
        const G4ThreeVector photon_x_axis = photon_y_axis.cross(photon_z_axis);

        G4cout
        << "\nphoton_x_axis: " << photon_x_axis
        << "\nphoton_y_axis: " << photon_y_axis
        << "\nphoton_z_axis: " << photon_z_axis
        << G4endl;

        const G4ThreeVector scattering_plane = (photon_z_axis.cross(postStepPoint->GetMomentum())).unit();
        G4cout << "\nScattering_plane: " << scattering_plane << G4endl;

        G4double phi_scattering = std::acos(scattering_plane*photon_y_axis);
        G4cout << "\nphi_scattering: " << phi_scattering << G4endl;

        fpRunAction->RecordData(TangleRunAction::Data(phi_scattering,0.));
      }
    }
  }
  
  return;
}
