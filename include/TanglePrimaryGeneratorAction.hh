#ifndef TanglePrimaryGeneratorAction_hh
#define TanglePrimaryGeneratorAction_hh 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4GeneralParticleSource.hh"

class G4GeneralParticleSource;

class TanglePrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  TanglePrimaryGeneratorAction();
  virtual ~TanglePrimaryGeneratorAction();

  virtual void GeneratePrimaries(G4Event*);

private:
  G4GeneralParticleSource*  fParticleGun;
};

#endif
