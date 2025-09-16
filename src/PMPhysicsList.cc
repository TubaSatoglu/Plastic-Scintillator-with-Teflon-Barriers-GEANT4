#include "PMPhysicsList.hh"

// Geant4 Core Physics Headers
#include "G4EmStandardPhysics.hh"
#include "G4OpticalPhysics.hh"
#include "G4DecayPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4HadronPhysicsQGSP_BERT_HP.hh"
#include "G4IonPhysics.hh"
#include "G4StoppingPhysics.hh"
#include "G4OpticalParameters.hh"

// Optical Physics
#include "G4OpticalPhoton.hh"
#include "G4ProcessManager.hh"
#include "G4Scintillation.hh"
#include "G4Cerenkov.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4OpticalPhysics.hh"

// Particle Constructors
#include "G4BosonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4ShortLivedConstructor.hh"

// Units
#include "G4SystemOfUnits.hh"

PMPhysicsList::PMPhysicsList() : G4VModularPhysicsList() {
    SetVerboseLevel(1);  // Debugging için seviye 1
    
    // ✅ Standart fizik süreçlerini ekle
    RegisterPhysics(new G4EmStandardPhysics());
    RegisterPhysics(new G4DecayPhysics());
    RegisterPhysics(new G4RadioactiveDecayPhysics());
    RegisterPhysics(new G4HadronPhysicsQGSP_BERT_HP());
    RegisterPhysics(new G4IonPhysics());
    RegisterPhysics(new G4StoppingPhysics());

    // ✅ Optik fizik ekle
    auto opticalPhysics = new G4OpticalPhysics();
    RegisterPhysics(opticalPhysics);

    // ✅ Optik süreç parametreleri
    auto* opticalParams = G4OpticalParameters::Instance();
    opticalParams->SetScintByParticleType(true);
    opticalParams->SetScintStackPhotons(true);
    opticalParams->SetScintTrackInfo(true);
    opticalParams->SetScintTrackSecondariesFirst(true);
    
    opticalParams->SetCerenkovStackPhotons(true);
    opticalParams->SetCerenkovMaxPhotonsPerStep(300);
    opticalParams->SetCerenkovMaxBetaChange(10.0);
    opticalParams->SetCerenkovTrackSecondariesFirst(true);

    G4cout << "✔ Physics List Loaded Successfully!\n" << G4endl;
}

PMPhysicsList::~PMPhysicsList() {}

void PMPhysicsList::ConstructParticle() {
    // ✅ Standart parçacıkları oluştur
    G4BosonConstructor bosons;
    bosons.ConstructParticle();

    G4LeptonConstructor leptons;
    leptons.ConstructParticle();

    G4MesonConstructor mesons;
    mesons.ConstructParticle();

    G4BaryonConstructor baryons;
    baryons.ConstructParticle();

    G4IonConstructor ions;
    ions.ConstructParticle();

    G4ShortLivedConstructor shortLived;
    shortLived.ConstructParticle();

    // ✅ Optik fotonları açıkça tanımla
    G4OpticalPhoton::OpticalPhotonDefinition();
}

void PMPhysicsList::ConstructProcess() {
    G4VModularPhysicsList::ConstructProcess();

    G4OpticalParameters* params = G4OpticalParameters::Instance();

    // ✅ Ensure scintillation process is properly configured
    params->SetScintByParticleType(true);
    params->SetScintStackPhotons(true);
    params->SetScintTrackInfo(true);
    params->SetScintTrackSecondariesFirst(true);

    params->SetCerenkovStackPhotons(true);
    params->SetCerenkovMaxPhotonsPerStep(300);
    params->SetCerenkovMaxBetaChange(10.0);
    params->SetCerenkovTrackSecondariesFirst(true);

    // ✅ **Scintillation Process**
    auto* scintillation = new G4Scintillation();
    scintillation->SetTrackSecondariesFirst(true);
    scintillation->SetVerboseLevel(1);  
    scintillation->SetScintillationByParticleType(true);

    auto* cerenkov = new G4Cerenkov();
    cerenkov->SetTrackSecondariesFirst(true);
    cerenkov->SetMaxNumPhotonsPerStep(100);  //cherekov'u sınırlamak için 500'den 100'e düşürdüm
    cerenkov->SetMaxBetaChangePerStep(10.0);

    // === Universal Scintillation Process Application ===
    auto scintProcess = new G4Scintillation("Scintillation");
    scintProcess->SetScintillationByParticleType(true);
    scintProcess->SetTrackSecondariesFirst(true);

    auto particleIterator = GetParticleIterator();
    particleIterator->reset();
    while ((*particleIterator)()) {
        G4ParticleDefinition* particle = particleIterator->value();
        G4ProcessManager* pmanager = particle->GetProcessManager();
        if (scintProcess->IsApplicable(*particle)) {
            pmanager->AddProcess(scintProcess);
            pmanager->SetProcessOrderingToLast(scintProcess, idxAtRest);
            pmanager->SetProcessOrderingToLast(scintProcess, idxPostStep);
        }
    }

    G4cout << "\n✔ Physics processes constructed via iterator.\n" << G4endl;
    G4cout << "\n=== Physics Process Construction ===" << G4endl;
    G4cout << "✔ Standard EM Physics enabled" << G4endl;
    G4cout << "✔ Optical Physics (Scintillation & Cerenkov) enabled" << G4endl;
    G4cout << "✔ Scintillation Process added to Gamma, Electrons, and Positrons" << G4endl;
    G4cout << "✔ Cerenkov Process added to Electrons and Positrons" << G4endl;
    G4cout << "=================================\n" << G4endl;
}

void PMPhysicsList::SetCuts() {
    SetCutsWithDefault();
    
    // ✅ Minimum üretim eşikleri (cut değerleri)
    SetCutValue(0.01 * mm, "gamma");
    SetCutValue(0.01 * mm, "e-");
    SetCutValue(0.01 * mm, "e+");
    SetCutValue(0.01 * mm, "proton");
    SetCutValue(0.01 * mm, "opticalphoton");

    G4cout << "\n=== Production Cuts Set ===" << G4endl;
    G4cout << "✔ Gamma cut: 0.01 mm" << G4endl;
    G4cout << "✔ Electron cut: 0.01 mm" << G4endl;
    G4cout << "✔ Positron cut: 0.01 mm" << G4endl;
    G4cout << "✔ Proton cut: 0.01 mm" << G4endl;
    G4cout << "✔ opticalphoton cut: 0.01 mm" << G4endl;
    G4cout << "====================================\n" << G4endl;
}
