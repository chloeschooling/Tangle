#ifndef TangleDetectorConstruction_hh
#define TangleDetectorConstruction_hh 1

#include "G4VUserDetectorConstruction.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

class TangleDetectorConstruction : public G4VUserDetectorConstruction
{
public:
  TangleDetectorConstruction();
  virtual ~TangleDetectorConstruction();

  virtual G4VPhysicalVolume* Construct();
  void ConstructSDandField();

private:
  G4LogicalVolume* fpWater_log;
};

#endif

