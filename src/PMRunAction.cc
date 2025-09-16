#include "PMRunAction.hh"
#include "G4AnalysisManager.hh"
#include "G4Run.hh"
#include <sstream>

// **Constructor (Enerji Parametreli)**
PMRunAction::PMRunAction(G4double energy) : fEnergy(energy) {
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    analysisManager->SetVerboseLevel(1);

    analysisManager->CreateH1("Edep", "Energy deposit", 100, 0., 1.1 * MeV);
    analysisManager->CreateH1("OpticalPhotons", "Optical Photon Count per Event", 100, 0, 500);
    analysisManager->CreateH1("GammaTeflon", "Gammas at Teflon Barrier per Event", 100, 0, 100);  // ✅ Güncellendi

    // **Optical Photon Data**
    analysisManager->CreateNtuple("Photons", "Optical Photons");
    analysisManager->CreateNtupleIColumn("iEvent");
    analysisManager->CreateNtupleDColumn("fX");
    analysisManager->CreateNtupleDColumn("fY");
    analysisManager->CreateNtupleDColumn("fZ");
    analysisManager->CreateNtupleDColumn("fGlobalTime");
    analysisManager->CreateNtupleDColumn("fWlen");
    analysisManager->CreateNtupleDColumn("Edep");
    analysisManager->FinishNtuple();
}

// **Destructor**
PMRunAction::~PMRunAction() {}

// **Run Başlangıcı**
void PMRunAction::BeginOfRunAction(const G4Run *run) {
    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
    std::stringstream filename;
    filename << "simulation_output_" << fEnergy << "MeV.root";
    analysisManager->SetFileName(filename.str());
    analysisManager->SetNtupleMerging(true);
    analysisManager->OpenFile();

    G4cout << "Run started with " << fEnergy << " MeV. Data saved in: " 
           << filename.str() << G4endl;
}

// **Run Sonu**
void PMRunAction::EndOfRunAction(const G4Run *run) {
    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
    if (analysisManager->IsActive()) {
        analysisManager->Write();
        analysisManager->CloseFile();
    }

    G4cout << "Run finished. Data saved in: simulation_output_" << fEnergy << "MeV.root" << G4endl;
}
