#include "TangleRunAction.hh"

#include "G4Run.hh"
#include "G4Threading.hh"
#include "G4AutoLock.hh"
#include <cassert>

TangleRunAction* TangleRunAction::fpMasterRunAction = 0;

TangleRunAction::TangleRunAction()
: fNPhotons(0)
{
  if (G4Threading::IsMasterThread()) {
    fpMasterRunAction = this;
  } else {
    // Worker thread.  fpMasterRunAction should have been initialised by now.
    assert(fpMasterRunAction);
  }
}

TangleRunAction::~TangleRunAction()
{}

void TangleRunAction::BeginOfRunAction(const G4Run*)
{
  fNPhotons = 0;
}

namespace {
  G4Mutex runActionMutex = G4MUTEX_INITIALIZER;
}

void TangleRunAction::EndOfRunAction(const G4Run* run)
{
  G4int nofEvents = run->GetNumberOfEvent();
  if (nofEvents == 0) return;

  G4String runType;
  if (G4Threading::IsMasterThread()) {
    runType = "Global Run";
  } else {
    runType = "Local Run-";
    // Merge to master counter
    G4AutoLock lock(&runActionMutex);  // For duration of scope.
    fpMasterRunAction->fNPhotons += fNPhotons;
  }

  G4cout
  << "\n----------------------End of " << runType << "------------------------"
  << "\n The run consists of " << nofEvents << " events."
  << "\n Number of photons reaching the sensitive detector: "
  << fNPhotons
  << "\n------------------------------------------------------------"
  << G4endl;
}
