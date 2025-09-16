#ifndef PMRUNACTION_HH
#define PMRUNACTION_HH

#include "G4UserRunAction.hh"
#include "G4Run.hh"
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"
#include "globals.hh"

class PMRunAction : public G4UserRunAction {
public:
    PMRunAction(G4double energy);  // ✅ Enerji parametresi alır
    ~PMRunAction();

    virtual void BeginOfRunAction(const G4Run* run);
    virtual void EndOfRunAction(const G4Run* run);

private:
    G4double fEnergy;  // ✅ Enerjiyi saklamak için değişken
};

#endif
