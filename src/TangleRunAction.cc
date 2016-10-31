#include "TangleRunAction.hh"

#include "G4Run.hh"
#include "G4Threading.hh"
#include "G4AutoLock.hh"
#include <cassert>
#include <fstream>

TangleRunAction* TangleRunAction::fpMasterRunAction = 0;
std::vector<std::vector<TangleRunAction::Data>*> TangleRunAction::fPointers;

namespace {
  G4Mutex runActionMutex = G4MUTEX_INITIALIZER;
}

TangleRunAction::TangleRunAction()
{
  if (G4Threading::IsMasterThread()) {
    fpMasterRunAction = this;
  } else {
    // Worker thread.  fpMasterRunAction should have been initialised by now.
    assert(fpMasterRunAction);
    fData.reserve(10000000);
    G4AutoLock lock(&runActionMutex);
    fpMasterRunAction->fPointers.push_back(&fData);
  }
}

TangleRunAction::~TangleRunAction()
{}

void TangleRunAction::BeginOfRunAction(const G4Run*)
{
  fData.clear();
}

void TangleRunAction::RecordDeltaPhi(const TangleRunAction::Data& data)
{
  fData.push_back(data);
}

namespace {
  std::ofstream outFile("outFile.csv");
}

void TangleRunAction::EndOfRunAction(const G4Run* run)
{
  G4int nofEvents = run->GetNumberOfEvent();
  if (nofEvents == 0) return;

  G4String runType;
  if (G4Threading::IsMasterThread()) {
    runType = "Global Run";
    // Workers finished...
    outFile << "#,phi1,phi2" << std::endl;
    for (const auto& pointer: fPointers) {
      for (const auto& data: *pointer) {
        outFile << ',' << data.fPhi1 << ',' << data.fPhi2 << std::endl;
      }
    }
  } else {
    runType = "Local Run-";
  }

  G4cout
  << "\n----------------------End of " << runType << "------------------------"
  << "\n The run consists of " << nofEvents << " events."
//  << "\n Number of photons reaching the sensitive detector: "
//  << fNPhotons
  << "\n------------------------------------------------------------"
  << G4endl;
}
