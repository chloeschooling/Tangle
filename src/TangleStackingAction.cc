//  TangleStackingAction.cc
//
//  Created by John Allison on 22/11/2016.

#include "TangleStackingAction.hh"
#include "G4StackManager.hh"

#include "G4VProcess.hh"
#include "G4Gamma.hh"

TangleStackingAction::TangleStackingAction()
: fStage(0)
, fFirstAnnihilation(true)
, fFirstPhoton(true)
, fParentID1(-1)
{}

TangleStackingAction::~TangleStackingAction()
{}

//#define TangleStackingActionPrinting

G4ClassificationOfNewTrack
TangleStackingAction::ClassifyNewTrack(const G4Track * track)
{
#ifdef TangleStackingActionPrinting
  G4cout << "TangleStackingAction::ClassifyNewTrack: fStage: " << fStage << G4endl;
#endif

  G4ClassificationOfNewTrack classification = fUrgent;

  const G4VProcess* creatorProcess = track->GetCreatorProcess();
  if (creatorProcess == nullptr) goto end;

  if (creatorProcess->GetProcessName() == "annihil" &&
      track->GetDefinition() == G4Gamma::Gamma()) {
    // For first stage, push annihilation photons onto waiting stack.
    if (fStage == 0) classification = fWaiting;
    else {
      // For subsequent stages push all but first annihilation onto waiting stack
      G4int parentID = track->GetParentID();
      if (fFirstAnnihilation) {
        if (fFirstPhoton) {
          fFirstPhoton = false;
          fParentID1 = parentID;
        } else {
          if (parentID == fParentID1) {
            // Its mate has been foound
            fFirstAnnihilation = false;
          } else {
            classification = fWaiting;
          }
        }
      } else {
        // Not first annihilation
        classification = fWaiting;
      }
    }
  }

end:

#ifdef TangleStackingActionPrinting
  G4cout
  << "\n  trackID: " << track->GetTrackID()
  << "\n  parentID: " << track->GetParentID()
  << "\n  classification: " << classification
  << G4endl;
#endif

  return classification;
}

void TangleStackingAction::NewStage()
{
  fStage++;

#ifdef TangleStackingActionPrinting
  G4cout << "TangleStackingAction::NewStage: fStage: " << fStage << G4endl;

  G4cout <<
  "\n  Particles other than annihilation photons have been tracked."
  "\n  The annihilation photons will have been transferred to the urgent stack,"
  "\n  so transfer them back to the waiting stack, then transfer one"
  "\n  back onto the urgent stack."
  << G4endl;

  G4cout << "stackManager->GetNUrgentTrack(): " << stackManager->GetNUrgentTrack() << G4endl;
#endif

  if(stackManager->GetNUrgentTrack())
  {
    // Send all tracks now in the urgent stack to ClassifyNewTrack so that
    // an annihilation pair is left on urgent stack.
    stackManager->ReClassify();
    // Reset flags for further annihilations, if any.
    fFirstAnnihilation = true;
    fFirstPhoton = true;
  }
}

void TangleStackingAction::PrepareNewEvent()
{
  fStage = 0;
  fFirstAnnihilation = true;
  fFirstPhoton = true;
}
