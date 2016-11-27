#ifndef TangleRunAction_hh
#define TangleRunAction_hh

#include "G4UserRunAction.hh"

#include <vector>

class G4Run;

class TangleRunAction : public G4UserRunAction
{
public:
  struct Data {
    Data(): fTheta1(0.), fPhi1(0.), fTheta2(0.), fPhi2(0.) {}
    Data(G4double theta1, G4double phi1, G4double theta2, G4double phi2):
    fTheta1(theta1), fPhi1(phi1), fTheta2(theta2), fPhi2(phi2) {}
    // Default copy constructor
    // Default assigment operator
    G4double fTheta1, fPhi1, fTheta2, fPhi2;
  };
  TangleRunAction();
  virtual ~TangleRunAction();

  virtual void BeginOfRunAction(const G4Run*);
  virtual void   EndOfRunAction(const G4Run*);
  
  void RecordData(const Data& data) {fData.push_back(data);}

private:
  static TangleRunAction* fpMasterRunAction;
  std::vector<Data> fData;
  static std::vector<std::vector<Data>*> fPointers;
};

#endif

