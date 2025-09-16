#include "PMPrimaryGenerator.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4Event.hh"
#include "Randomize.hh"
#include "G4Gamma.hh"
#include "G4PhysicalConstants.hh"

PMPrimaryGenerator::PMPrimaryGenerator(G4double energy) {
    fParticleGun = new G4ParticleGun(1);

    // Set up gamma source
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* particle = particleTable->FindParticle("gamma");
    fParticleGun->SetParticleDefinition(particle);

    // Use the provided energy instead of a default value
    fBaseEnergy = energy;
    fEnergySigma = 0.1 * keV;

    // Source position parameters
    fSourcePosZ = 0.0 * cm;
    fSourceRadius = 2.0 * mm;
    fBeamSpreadAngle = 5.0 * deg;

    G4cout << "✅ Gamma energy set to " << fBaseEnergy / MeV << " MeV (from input)" << G4endl;
}

PMPrimaryGenerator::~PMPrimaryGenerator() {
    delete fParticleGun;
}

void PMPrimaryGenerator::UpdatePositionAndDirection() {
    G4double r = fSourceRadius * std::sqrt(G4UniformRand());
    G4double phi = 2.0 * pi * G4UniformRand();
    G4double x = r * std::cos(phi);
    G4double y = r * std::sin(phi);
    
    fParticleGun->SetParticlePosition(G4ThreeVector(x, y, fSourcePosZ));

    G4double theta = fBeamSpreadAngle * std::sqrt(G4UniformRand());
    G4double dirPhi = 2.0 * pi * G4UniformRand();
    
    G4double dx = std::sin(theta) * std::cos(dirPhi);
    G4double dy = std::sin(theta) * std::sin(dirPhi);
    G4double dz = std::cos(theta);
    
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(dx, dy, dz));

    // Generate energy within Gaussian spread
    G4double energy = G4RandGauss::shoot(fBaseEnergy, fEnergySigma);
    energy = std::max(energy, 1 * keV);

    fParticleGun->SetParticleEnergy(energy);
}

void PMPrimaryGenerator::GeneratePrimaries(G4Event* anEvent) {
    fParticleGun->SetParticleDefinition(G4Gamma::GammaDefinition());

    // Update position and direction
    UpdatePositionAndDirection();
    fParticleGun->GeneratePrimaryVertex(anEvent);

    G4cout << "🔹 Generating gamma with energy: " << fParticleGun->GetParticleEnergy() / MeV << " MeV" << G4endl;
}
