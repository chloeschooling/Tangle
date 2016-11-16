#ifndef TangleRunAction_hh
#define TangleRunAction_hh

#include "G4UserRunAction.hh"

#include <vector>

class G4Run;

class TangleRunAction : public G4UserRunAction
{
public:
  struct Data {
    Data(G4double phi1, G4double phi2)
    : fPhi1(phi1), fPhi2(phi2) {}
    G4double fPhi1;
    G4double fPhi2;
  };
  TangleRunAction();
  virtual ~TangleRunAction();

  virtual void BeginOfRunAction(const G4Run*);
  virtual void   EndOfRunAction(const G4Run*);
  
  void RecordData(const Data& data);

private:
  static TangleRunAction* fpMasterRunAction;
  std::vector<Data> fData;
  static std::vector<std::vector<Data>*> fPointers;
};

#endif

