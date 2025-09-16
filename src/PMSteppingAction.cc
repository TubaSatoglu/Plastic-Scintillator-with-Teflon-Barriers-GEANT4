#include "PMSteppingAction.hh"
#include "PMEventAction.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4OpticalPhoton.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4SystemOfUnits.hh"
#include "G4VPhysicalVolume.hh"

// Constructor
PMSteppingAction::PMSteppingAction(PMEventAction* eventAction)
 : G4UserSteppingAction(),
   fEventAction(eventAction)
{}

// Destructor
PMSteppingAction::~PMSteppingAction() {}

void PMSteppingAction::UserSteppingAction(const G4Step* step) {
    auto* track = step->GetTrack();
    auto* particle = track->GetDefinition();
    
    // ========================================
    // OPTIK FOTON TAKİBİ VE ALUMINUM KONTROLÜ
    // ========================================
    if (track->GetDefinition() == G4OpticalPhoton::OpticalPhoton()) {
        auto* process = step->GetPostStepPoint()->GetProcessDefinedStep();
        G4String processName = process ? process->GetProcessName() : "Unknown";
        G4String volumeName = step->GetPreStepPoint()->GetPhysicalVolume()->GetName();
        
        // Yeni üretilen optik fotonları logla
        if (step->GetTrack()->GetCurrentStepNumber() == 1) {
            G4cout << "🔆 Optical photon created by: " << processName
                   << " in volume: " << volumeName << G4endl;
        }
        
        // 🔥 ALUMINUM VOLUME KONTROLÜ
        if (volumeName == "physAlDetector") {
            G4cout << "⚡ Optical photon ENTERING Aluminum detector!" << G4endl;
        }
        
        // Aluminum'dan çıkan fotonları da kontrol et
        G4String postVolumeName = "None";
        if (step->GetPostStepPoint()->GetPhysicalVolume()) {
            postVolumeName = step->GetPostStepPoint()->GetPhysicalVolume()->GetName();
        }
        
        if (volumeName == "physAlDetector" && postVolumeName != "physAlDetector") {
            G4cout << "🔥 Optical photon EXITING Aluminum detector!" << G4endl;
        }
        
        // 🎯 Aluminum yakınındaki fotonları takip et (debug için)
        G4ThreeVector position = step->GetPreStepPoint()->GetPosition();
        G4double offset = 5.01*cm; // DetectorConstruction'daki offset değeri
        G4double teflonThickness = 0.2*cm; // DetectorConstruction'daki değer
        G4double alPosX = -offset - 0.5*teflonThickness - 0.5*mm - 0.5*(0.01*cm);
        
        if (abs(position.x() - alPosX) < 1*cm) {  // 1 cm yakınlık
            G4cout << "🎯 Optical photon near aluminum: " 
                   << "pos=(" << position.x()/mm << "," << position.y()/mm << "," << position.z()/mm << ") mm, "
                   << "volume=" << volumeName << G4endl;
        }
    }
    
    // ========================================
    // NON-OPTICAL PARTICLE ENERJİ KONTROLÜ
    // ========================================
    if (particle != G4OpticalPhoton::Definition()) {
        G4double edep = step->GetTotalEnergyDeposit() / MeV;
        if (edep > 0.0 && fEventAction) {
            fEventAction->RecordEnergy(edep);
        }
        
        // --- Optik dışı parçacıklar için debug logu ---
        auto* preStep = step->GetPreStepPoint();
        auto* postStep = step->GetPostStepPoint();
        
        G4String pname = particle->GetParticleName();
        G4double ekin = preStep->GetKineticEnergy() / MeV;
        auto* vol = preStep->GetPhysicalVolume();
        G4String vname = vol ? vol->GetName() : "None";
        G4String procName = postStep->GetProcessDefinedStep()
                           ? postStep->GetProcessDefinedStep()->GetProcessName()
                           : "None";
        
        G4cout << "\n=== Step Info ===\n"
               << "Particle: " << pname << "\n"
               << "Kinetic: " << ekin << " MeV\n"
               << "Volume: " << vname << "\n"
               << "Process: " << procName << "\n"
               << "Edep: " << edep << " MeV\n"
               << "================\n";
    }
}
