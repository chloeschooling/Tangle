//  TangleStackingAction.cc
//
//  Created by John Allison on 22/11/2016.

#include "TangleStackingAction.hh"
#include "G4StackManager.hh"

#include "G4VProcess.hh"

TangleStackingAction::TangleStackingAction()
: fStage(0)
, fFirstAnnihilation(true)
, fFirstPhoton(true)
{}

TangleStackingAction::~TangleStackingAction()
{}

G4ClassificationOfNewTrack
TangleStackingAction::ClassifyNewTrack(const G4Track * track)
{
//  G4cout << "TangleStackingAction::ClassifyNewTrack: fStage: " << fStage << G4endl;

  G4ClassificationOfNewTrack classification = fUrgent;

  const G4VProcess* creatorProcess = track->GetCreatorProcess();
  if (creatorProcess == nullptr) return classification;

  if (creatorProcess->GetProcessName() == "annihil") {
    // For first stage, push annihilation photons onto waiting stack.
    if (fStage == 0) classification = fWaiting;
    else {
      // For subsequent stages push all but first annihilation onto waiting stack
      G4int parentID = track->GetParentID();
      static G4int parentID1 = -1;
      if (fFirstAnnihilation) {
        if (fFirstPhoton) {
          fFirstPhoton = false;
          parentID1 = parentID;
        } else {
          if (parentID == parentID1) {
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

  return classification;
}

void TangleStackingAction::NewStage()
{
  fStage++;

//  G4cout << "TangleStackingAction::NewStage: fStage: " << fStage << G4endl;

//  G4cout <<
//  "\n  Particles other than annihilation photons have been tracked."
//  "\n  The annihilation photons will have been transferred to the urgent stack,"
//  "\n  so transfer them back to the waiting stack, then transfer one"
//  "\n  back onto the urgent stack."
//  << G4endl;

//  G4cout << "stackManager->GetNUrgentTrack(): " << stackManager->GetNUrgentTrack() << G4endl;

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
