#ifndef PMACTIONINITIALIZATION_HH
#define PMACTIONINITIALIZATION_HH

#include "G4VUserActionInitialization.hh"
#include "G4SystemOfUnits.hh"  
#include "PMPrimaryGenerator.hh"
#include "PMRunAction.hh"

class PMActionInitialization : public G4VUserActionInitialization {
public:
    explicit PMActionInitialization(G4double energy);  // ✅ Constructor doğru tanımlandı
    ~PMActionInitialization() override;

    void BuildForMaster() const override;
    void Build() const override;

private:
    G4double fEnergy;  // ✅ fEnergy değişkeni doğru tanımlandı
};

#endif  // PMACTIONINITIALIZATION_HH
