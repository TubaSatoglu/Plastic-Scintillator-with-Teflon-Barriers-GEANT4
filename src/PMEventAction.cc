#include "PMEventAction.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"

PMEventAction::PMEventAction()
    : G4UserEventAction(),
      fOpticalPhotonCount(0),
      fGammaTeflonCount(0),
      fAluminumPhotonCount(0),  // ✅ Alüminyum için sayaç eklendi
      fScintillationCount(0),
      fTotalEnergyDep(0.) {
}

PMEventAction::~PMEventAction() {}

void PMEventAction::BeginOfEventAction(const G4Event*) {
    fOpticalPhotonCount = 0;
    fGammaTeflonCount = 0;
    fAluminumPhotonCount = 0; // ✅ Başlangıçta sıfırla
    fScintillationCount = 0;
    fTotalEnergyDep = 0.;
    fEnergyDeposits.clear();
}

void PMEventAction::EndOfEventAction(const G4Event* event) {
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

    analysisManager->FillH1(0, fOpticalPhotonCount);
    analysisManager->FillH1(1, fGammaTeflonCount);
    analysisManager->FillH1(2, fAluminumPhotonCount); // ✅ Histogram eklendi
    analysisManager->FillH1(3, fScintillationCount);
    analysisManager->FillH1(4, fTotalEnergyDep / MeV);

    analysisManager->FillNtupleIColumn(0, event->GetEventID());
    analysisManager->FillNtupleIColumn(1, fOpticalPhotonCount);
    analysisManager->FillNtupleIColumn(2, fGammaTeflonCount);
    analysisManager->FillNtupleIColumn(3, fAluminumPhotonCount); // ✅ Ntuple eklendi
    analysisManager->FillNtupleIColumn(4, fScintillationCount);
    analysisManager->FillNtupleDColumn(5, fTotalEnergyDep / MeV);
    analysisManager->AddNtupleRow();

    G4cout << "\n====== Event " << event->GetEventID() << " Summary ======\n"
           << "💡 Total Optical Photons: " << fOpticalPhotonCount << "\n"
           << "🔹 Optical Photons Detected at Aluminum: " << fAluminumPhotonCount << "\n"
           << "🔎 Energy Deposited in NaI: " << fTotalEnergyDep << " MeV\n"
           << "🔹 Gammas at Teflon Barrier: " << fGammaTeflonCount << "\n"
           << "==========================================\n" << G4endl;

    if (fOpticalPhotonCount == 0) {
        G4cout << "⚠️ WARNING: Event " << event->GetEventID()
               << " produced **NO** optical photons!" << G4endl;
    }
}

void PMEventAction::AddOpticalPhoton() {
    fOpticalPhotonCount++;
    const G4Event* event = G4EventManager::GetEventManager()->GetConstCurrentEvent();
    
    // ✅ Terminalde her optik foton üretildiğinde mesaj göster
    G4cout << "💡 Optical Photon Created in Event " << event->GetEventID()
           << " | Count: " << fOpticalPhotonCount << G4endl;
}

void PMEventAction::AddGammaToTeflon() {
    fGammaTeflonCount++;
}

void PMEventAction::AddAluminumPhoton() {  
    fAluminumPhotonCount++;
    fPhotonsAtAluminumBoundary++;
    G4cout << " Optical Photon Detected at Aluminum! Count: " << fAluminumPhotonCount << G4endl;
}

void PMEventAction::AddScintillationPhoton() {
    fScintillationCount++;
}

void PMEventAction::RecordEnergy(G4double energy) {
    fTotalEnergyDep += energy;
    fEnergyDeposits.push_back(energy);
}
