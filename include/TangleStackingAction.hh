//  TangleStackingAction.hh
//
//  Created by John Allison on 22/11/2016.

#ifndef TangleStackingAction_hh
#define TangleStackingAction_hh

#include "G4UserStackingAction.hh"
#include "globals.hh"

class G4Track;

class TangleStackingAction: public G4UserStackingAction
{
public:
  TangleStackingAction();
  virtual ~TangleStackingAction();

public:
  virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track*);
  virtual void NewStage();
  virtual void PrepareNewEvent();

private:
  G4int fStage;
  G4bool fFirstAnnihilation;
  G4bool fFirstPhoton;

  // This is for remembering something about the first photon.
  G4int fParentID1;
};

#endif /* TangleStackingAction_hh */
