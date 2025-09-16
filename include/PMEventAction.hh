#ifndef PMEVENTACTION_HH
#define PMEVENTACTION_HH

#include "G4UserEventAction.hh"
#include "G4Types.hh"        
#include <vector>            
#include "globals.hh"        

class G4Event;

class PMEventAction : public G4UserEventAction {
public:
    PMEventAction();
    virtual ~PMEventAction();

    virtual void BeginOfEventAction(const G4Event*);
    virtual void EndOfEventAction(const G4Event*);
    
    void AddOpticalPhoton();
    void AddGammaToTeflon();
    void AddAluminumPhoton();  // Bu fonksiyon fAluminumPhotonCount ve fPhotonsAtAluminumBoundary'ı artırıyor.
    void AddScintillationPhoton();
    void RecordEnergy(G4double energy);

private:
    G4int fOpticalPhotonCount;
    G4int fGammaTeflonCount;
    G4int fAluminumPhotonCount;
    G4int fPhotonsAtAluminumBoundary;  // Yeni sayaç ekleyin!
    G4int fScintillationCount;
    G4double fTotalEnergyDep;
    std::vector<G4double> fEnergyDeposits;
};

#endif
