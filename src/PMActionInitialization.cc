#include "PMActionInitialization.hh"
#include "PMPrimaryGenerator.hh"
#include "PMRunAction.hh"
#include "G4SystemOfUnits.hh"  // ✅ G4double kullanımı için eklendi!

PMActionInitialization::PMActionInitialization(G4double energy)
    : G4VUserActionInitialization(), fEnergy(energy) {}  // ✅ Constructor artık doğru tanımlandı.

PMActionInitialization::~PMActionInitialization() = default;

void PMActionInitialization::BuildForMaster() const {
    SetUserAction(new PMRunAction(fEnergy));  // ✅ fEnergy parametresi artık doğru geçiriliyor.
}

void PMActionInitialization::Build() const {
    SetUserAction(new PMPrimaryGenerator(fEnergy));  // ✅ fEnergy parametresi doğru geçirildi.
    SetUserAction(new PMRunAction(fEnergy));        // ✅ Eksik parametre hatası giderildi.
}
