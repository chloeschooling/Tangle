//  TangleStackingAction.cc
//
//  Created by John Allison on 22/11/2016.

#include "TangleStackingAction.hh"
#include "G4StackManager.hh"

#include "G4VProcess.hh"

TangleStackingAction::TangleStackingAction()
:fStage(0)
{}

TangleStackingAction::~TangleStackingAction()
{}

G4ClassificationOfNewTrack
TangleStackingAction::ClassifyNewTrack(const G4Track * track)
{
  G4cout << "TangleStackingAction::ClassifyNewTrack: fStage: " << fStage << G4endl;

  G4ClassificationOfNewTrack classification = fUrgent;

  const G4VProcess* creatorProcess = track->GetCreatorProcess();
  if (creatorProcess == nullptr) return classification;

  // At any stage, push annihilation photons onto waiting stack.
  if (creatorProcess->GetProcessName() == "annihil") {
    classification = fWaiting;
  }

  return classification;
}

void TangleStackingAction::NewStage()
{
  G4cout << "TangleStackingAction::NewStage: fStage: " << fStage << G4endl;

  G4cout <<
  "\n  Particles other than annihilation photons have been tracked."
  "\n  The annihilation photons will have been transferred to the urgent stack,"
  "\n  so transfer them back to the waiting stack, then transfer one"
  "\n  back onto the urgent stack."
  << G4endl;

  G4cout << "stackManager->GetNUrgentTrack(): " << stackManager->GetNUrgentTrack() << G4endl;

  if(stackManager->GetNUrgentTrack())
  {
    // Transfer all tracks in Urgent stack to Waiting stack, since all tracks
    // in Waiting stack have already been transfered to Urgent stack before
    // invokation of this method.
    stackManager->TransferStackedTracks(fUrgent,fWaiting);

    // Then, transfer only one track to Urgent stack.
    stackManager->TransferOneStackedTrack(fWaiting,fUrgent);
    /////////////////// PROBLEM ////////////////
    // We want the order in which photons are delivered to be two at a time
    // from the same annihilation but the above mixes the order if there are
    // more than one annihilation per event.
    /////////////////// PROBLEM ////////////////
  }

  fStage++;
}

void TangleStackingAction::PrepareNewEvent()
{
  fStage = 0;
}
