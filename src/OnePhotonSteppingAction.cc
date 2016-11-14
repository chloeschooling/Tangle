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

  G4Track* track = step->GetTrack();
  const G4VProcess* creatorProcess = track->GetCreatorProcess();
  if (creatorProcess == nullptr) return;

  G4StepPoint* preStepPoint = step->GetPreStepPoint();

  G4StepPoint* postStepPoint = step->GetPostStepPoint();
  const G4VProcess* postProcessDefinedStep = postStepPoint->GetProcessDefinedStep();
  if (postProcessDefinedStep == nullptr) return;

  if (postProcessDefinedStep->GetProcessName() == "compt") {
    // This is a Compton scatter.
    if (!fComptonScatteringFound) {
      if (track->GetCurrentStepNumber() == 1) {
        fComptonScatteringFound = true;
        // This is the first Compton scatter
        G4cout
        << "Compton scatter found: trackID: " << track->GetTrackID()
        << ", preStepPointPosition: " << preStepPoint->GetPosition()
        << ", postStepPointPosition: " << postStepPoint->GetPosition()
        << ", preStepPointMomentum: " << preStepPoint->GetMomentum()
        << ", postStepPointMomentum: " << postStepPoint->GetMomentum()
        << ", preSteppolarisation: " << preStepPoint->GetPolarization()
        << ", postSteppolarisation: " << postStepPoint->GetPolarization()
        << G4endl;
        G4cout << "Scattering plane: "
        << preStepPoint->GetMomentum().cross(postStepPoint->GetMomentum()).unit()
        << G4endl;
      }
    }
  }

  return;
}
