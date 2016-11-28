#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif
#include "G4PhysListFactory.hh"
#include "TangleDetectorConstruction.hh"
#include "G4EmLivermorePolarizedPhysics.hh"
#include "TangleActionInitialization.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"

int main(int argc,char** argv)
{
  // Do this first to capture all output
  G4UIExecutive* ui = new G4UIExecutive(argc, argv);

#ifdef G4MULTITHREADED
  G4MTRunManager* runManager = new G4MTRunManager;
#else
  G4RunManager* runManager = new G4RunManager;
#endif

  runManager->SetUserInitialization(new TangleDetectorConstruction);

  G4int verbose;
  G4PhysListFactory factory;
  G4VModularPhysicsList* physList = factory.GetReferencePhysList("FTFP_BERT");
  physList->SetVerboseLevel(verbose = 1);
  physList->ReplacePhysics(new G4EmLivermorePolarizedPhysics);
  runManager->SetUserInitialization(physList);

  runManager->SetUserInitialization(new TangleActionInitialization);

  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();

  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  UImanager->ApplyCommand("/control/execute vis.mac");

  ui->SessionStart();

  delete ui;
  delete visManager;
  delete runManager;
}
