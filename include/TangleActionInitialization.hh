#ifndef TangleActionInitialization_hh
#define TangleActionInitialization_hh 1

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


