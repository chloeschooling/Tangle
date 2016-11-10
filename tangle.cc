#include "G4MTRunManager.hh"
#include "G4PhysListFactory.hh"
#include "TangleDetectorConstruction.hh"
#include "QBBC.hh"
#include "G4EmLivermorePolarizedPhysics.hh"
#include "G4EmLowEPPolarizedPhysics.hh"
#include "TangleActionInitialization.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"

int main(int argc,char** argv)
{
  // Do this first to capture all output
  G4UIExecutive* ui = new G4UIExecutive(argc, argv);

  G4RunManager* runManager = new G4MTRunManager;

  runManager->SetUserInitialization(new TangleDetectorConstruction);

  G4int verbose;
  G4PhysListFactory factory;
  G4VModularPhysicsList* physList = factory.GetReferencePhysList("FTFP_BERT");
  physList->SetVerboseLevel(verbose = 1);
  physList->ReplacePhysics(new G4EmLivermorePolarizedPhysics);
//  physList->ReplacePhysics(new G4EmLowEPPolarizedPhysics);
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
