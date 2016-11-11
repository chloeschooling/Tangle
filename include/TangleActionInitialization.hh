#ifndef TangleActionInitialization_h
#define TangleActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

class TangleActionInitialization : public G4VUserActionInitialization
{
public:
  TangleActionInitialization();
  virtual ~TangleActionInitialization();

  virtual void BuildForMaster() const;
  virtual void Build() const;
};

#endif


