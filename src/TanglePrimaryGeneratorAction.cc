#include "TanglePrimaryGeneratorAction.hh"

#include "G4GeneralParticleSource.hh"

TanglePrimaryGeneratorAction::TanglePrimaryGeneratorAction()
{
  fParticleGun  = new G4GeneralParticleSource;
}

TanglePrimaryGeneratorAction::~TanglePrimaryGeneratorAction()
{
  delete fParticleGun;
}

void TanglePrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  fParticleGun->GeneratePrimaryVertex(anEvent);
}
