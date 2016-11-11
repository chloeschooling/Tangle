#ifndef TangleDetectorConstruction_h
#define TangleDetectorConstruction_h 1

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

