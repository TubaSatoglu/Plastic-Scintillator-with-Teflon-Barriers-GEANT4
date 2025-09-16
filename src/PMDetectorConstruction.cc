#include "PMDetectorConstruction.hh"
#include "G4SDManager.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4SubtractionSolid.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4LogicalSkinSurface.hh"


PMDetectorConstruction::PMDetectorConstruction()
{}

PMDetectorConstruction::~PMDetectorConstruction()
{}

// ============================
// Optik yüzey tanımı
// ============================
G4Material* PMDetectorConstruction::CreateAirMaterial() {
    G4NistManager* nist = G4NistManager::Instance();
    G4Material* air = nist->FindOrBuildMaterial("G4_AIR");

    G4MaterialPropertiesTable* airMPT = new G4MaterialPropertiesTable();
    G4double airPhotonEnergy[] = {2.0 * eV, 3.5 * eV};
    G4double airRindex[] = {1.0, 1.0};
    airMPT->AddProperty("RINDEX", airPhotonEnergy, airRindex, 2);

    air->SetMaterialPropertiesTable(airMPT);
    return air;
}
    
G4Material* PMDetectorConstruction::CreateScintillatorMaterial() {
    G4double a, z, density;
    G4int nelements;

    G4Element* Na = new G4Element("Sodium", "Na", z=11., a=22.99*g/mole);
    G4Element* I  = new G4Element("Iodine", "I", z=53., a=126.90*g/mole);

    G4Material* NaI = new G4Material("G4_SODIUM_IODIDE", density=3.67*g/cm3, nelements=2);
    NaI->AddElement(Na, 1);
    NaI->AddElement(I, 1);

    // --- Optik özellikler ---
    const G4int nOpticalPoints = 2;
    G4double photonEnergy[nOpticalPoints] = { 2.0*eV, 3.5*eV };
    G4double rindex[nOpticalPoints] = { 1.78, 1.78 };
    G4double scintSpectrum[nOpticalPoints] = { 1.0, 1.0 }; // normalize spektrum
    
    // --- Particle energy range for scintillation yield (much wider range) ---
    const G4int nEnergyPoints = 3;
    G4double particleEnergy[nEnergyPoints] = { 1.0*keV, 1.0*MeV, 10.0*MeV };
    
    G4double scintYield[nEnergyPoints] = { 50.0/MeV, 50.0/MeV, 50.0/MeV };        // Daha da azaltıldı
    G4double electronScintYield[nEnergyPoints] = { 45.0/MeV, 45.0/MeV, 45.0/MeV }; // Daha da azaltıldı  
    G4double protonScintYield[nEnergyPoints] = { 30.0/MeV, 30.0/MeV, 30.0/MeV };   // Daha da azaltıldı

    G4MaterialPropertiesTable* NaIMPT = new G4MaterialPropertiesTable();
    
    // Optical properties use photon energies
    NaIMPT->AddProperty("RINDEX", photonEnergy, rindex, nOpticalPoints);
    NaIMPT->AddProperty("SCINTILLATIONCOMPONENT1", photonEnergy, scintSpectrum, nOpticalPoints);

    // Scintillation yields use particle energies (wider range)
    NaIMPT->AddProperty("SCINTILLATIONYIELD", particleEnergy, scintYield, nEnergyPoints, true);
    NaIMPT->AddProperty("ELECTRONSCINTILLATIONYIELD", particleEnergy, electronScintYield, nEnergyPoints, true);
    NaIMPT->AddProperty("PROTONSCINTILLATIONYIELD", particleEnergy, protonScintYield, nEnergyPoints, true);

    // --- Ek parametreler ---
    NaIMPT->AddConstProperty("RESOLUTIONSCALE", 1.0);
    NaIMPT->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 250.*ns);
    NaIMPT->AddConstProperty("SCINTILLATIONYIELD1", 1.0);

    NaI->SetMaterialPropertiesTable(NaIMPT);
    return NaI;
}

G4Material* PMDetectorConstruction::CreateTeflonMaterial() {
    G4NistManager* nist = G4NistManager::Instance();
    G4Material* teflon = nist->FindOrBuildMaterial("G4_TEFLON");

    // Teflon malzemesi sadece kırılma indisine sahip olacak
    // Yansıtma özellikleri optik yüzey üzerinden tanımlanacak
    const G4int nEntries = 2;
    G4double photonEnergy[nEntries] = {2.0*eV, 3.5*eV};
    G4double rindex[nEntries] = {1.35, 1.35}; // Teflon kırılma indisi

    G4MaterialPropertiesTable* teflonMPT = new G4MaterialPropertiesTable();
    teflonMPT->AddProperty("RINDEX", photonEnergy, rindex, nEntries);

    teflon->SetMaterialPropertiesTable(teflonMPT);
    return teflon;
}


G4VPhysicalVolume *PMDetectorConstruction::Construct() {
    G4bool checkOverlaps = true;

    G4Material* airMat = CreateAirMaterial();
    G4Material* NaI = CreateScintillatorMaterial();
    G4Material* teflonMat = CreateTeflonMaterial();
    
    // Yansıtıcı optik yüzey oluştur
    G4OpticalSurface* reflectiveSurface = new G4OpticalSurface("ReflectiveSurface");
    reflectiveSurface->SetType(dielectric_metal);
    reflectiveSurface->SetFinish(polished);
    reflectiveSurface->SetModel(unified);

    const G4int nEntries = 2;
    G4double photonEnergy[nEntries] = {2.0*eV, 3.5*eV};
    G4double reflectivity[nEntries] = {1.0, 1.0}; // %100 yansıtıcılık
    G4double efficiency[nEntries] = {0.0, 0.0};   // Absorpsiyon yok

    G4MaterialPropertiesTable* surfaceMPT = new G4MaterialPropertiesTable();
    surfaceMPT->AddProperty("REFLECTIVITY", photonEnergy, reflectivity, nEntries);
    surfaceMPT->AddProperty("EFFICIENCY", photonEnergy, efficiency, nEntries);

    reflectiveSurface->SetMaterialPropertiesTable(surfaceMPT);
    
    G4double xWorld = 50.0 * cm;
    G4double yWorld = 50.0 * cm;
    G4double zWorld = 50.0 * cm;
    
    G4Box* solidWorld = new G4Box("solidWorld", 0.5*xWorld, 0.5*yWorld, 0.5*zWorld);
    G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, airMat, "logicWorld");
    G4VPhysicalVolume *physWorld = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), logicWorld, "physWorld", 0, false, 0, checkOverlaps);

    // ================================
    // Scintilatör
    // ================================
    G4double detectorSize = 10.0 * cm;
    G4Box* solidDetector = new G4Box("solidDetector", 0.5 * detectorSize, 0.5 * detectorSize, 0.5 * detectorSize);
    logicDetector = new G4LogicalVolume(solidDetector, NaI, "logicDetector");
    G4VPhysicalVolume *physDetector = new G4PVPlacement(0, G4ThreeVector(0., 0. , 10.5 * cm), 
                                                        logicDetector, "physDetector", logicWorld, 
                                                        false, 0, checkOverlaps);

    auto detVisAtt = new G4VisAttributes(G4Color(1.0, 1.0, 0.0, 1.0));
    detVisAtt->SetForceSolid(true);
    logicDetector->SetVisAttributes(detVisAtt);

    // ================================
    // Teflon plakalar (6 yüz için)
    // ================================
    G4double teflonThickness = 0.1 * cm;
    G4double offset = 0.5 * detectorSize + 0.5 * teflonThickness;
    
    G4VisAttributes *teflonVisAtt = new G4VisAttributes(G4Color(1.0, 0.0, 0.0, 0.5));
    teflonVisAtt->SetForceSolid(true);  

    // ================
    // +X yönü (delik yok - tam yansıtıcı)
    // ================
    G4Box *solidTeflonX = new G4Box("solidTeflonX", 
                                0.5 * teflonThickness, 
                                0.5 * detectorSize, 
                                0.5 * detectorSize);
    G4LogicalVolume *logicTeflonX = new G4LogicalVolume(solidTeflonX, teflonMat, "logicTeflonX");
    logicTeflonX->SetVisAttributes(teflonVisAtt);
    
    auto physTeflonXplus = new G4PVPlacement(0, G4ThreeVector(+offset,0.,10.5*cm), 
                            logicTeflonX, "physTeflonXplus", 
                            logicWorld, false, 0, checkOverlaps);
    
    // X+ yüzeyine optik yüzey tanımla (sintilatöre bakan yüzey)
    new G4LogicalBorderSurface("TeflonXplus_Surface", physDetector, physTeflonXplus, reflectiveSurface);


    // ================
    // X− plakası (delikli)
    // ================
    G4Box *solidTeflonXminus = new G4Box("solidTeflonXminus", 
                                        0.5 * teflonThickness, 
                                        0.5 * detectorSize, 
                                        0.5 * detectorSize);

    // Delik: 1 cm çapında, plaka kalınlığı kadar uzun
    G4Tubs* holeXminus = new G4Tubs("holeXminus", 
                                        0.*cm, 
                                        0.5*cm, 
                                        0.5*teflonThickness + 0.05*cm, 
                                        0.*deg, 
                                        360.*deg);

    // Rotasyon: silindiri X yönüne çevirmek için Y ekseni etrafında 90°
    G4RotationMatrix* rotX = new G4RotationMatrix();
    rotX->rotateY(90.*deg);

    // Çıkarma işlemi
    G4SubtractionSolid* solidTeflonXminusHole = new G4SubtractionSolid("solidTeflonXminusHole",
                                                solidTeflonXminus,
                                                holeXminus,
                                                rotX,
                                                G4ThreeVector(0.,0.,0.001*cm)); 

    // Logical volume
    G4LogicalVolume *logicTeflonXminus = new G4LogicalVolume(solidTeflonXminusHole, teflonMat, "logicTeflonXminus");
    logicTeflonXminus->SetVisAttributes(teflonVisAtt);

    // Yerleştirme
    auto physTeflonXminus = new G4PVPlacement(0, G4ThreeVector(-offset,0.,10.5*cm), 
                                                logicTeflonXminus, 
                                                "physTeflonXminus", 
                                                logicWorld, 
                                                false, 0, 
                                                checkOverlaps);
    
    // X- yüzeyine optik yüzey tanımla (sintilatöre bakan yüzey - sadece delik olmayan kısım)
    new G4LogicalBorderSurface("TeflonXminus_Surface", physDetector, physTeflonXminus, reflectiveSurface);


    // ================
    // Air Hole in (X-)
    // ================
    G4Tubs* airHoleXSolid = new G4Tubs("airHoleXSolid", 
                                        0.*cm, 
                                        0.5*cm, 
                                        0.5*teflonThickness + 0.05*cm, 
                                        0.*deg, 
                                        360.*deg);

    G4ThreeVector posAirHoleX = G4ThreeVector(-offset, 0., 10.5*cm); // Teflon X- pozisyonu ile aynı

    G4RotationMatrix* rotAirX = new G4RotationMatrix();
    rotAirX->rotateY(90.*deg);

    G4LogicalVolume* logicAirHoleX = new G4LogicalVolume(airHoleXSolid, airMat, "logicAirHoleX");

    auto physAirHoleX = new G4PVPlacement(rotAirX,
                                            posAirHoleX,
                                            logicAirHoleX,
                                            "physAirHoleX",
                                            logicWorld,
                                            false,
                                            0,
                                            checkOverlaps);


    // ================            
    // Y yönü (delik yok - tam yansıtıcı)
    // ================
    G4Box *solidTeflonY = new G4Box("solidTeflonY", 
                                    0.5 * detectorSize, 
                                    0.5 * teflonThickness, 
                                    0.5 * detectorSize);
    G4LogicalVolume *logicTeflonY = new G4LogicalVolume(solidTeflonY, teflonMat, "logicTeflonY");
    logicTeflonY->SetVisAttributes(teflonVisAtt);

    auto physTeflonYplus = new G4PVPlacement(0, G4ThreeVector(0.,+offset,10.5*cm), 
                        logicTeflonY, "physTeflonYplus", logicWorld, false, 0, checkOverlaps);
    auto physTeflonYminus = new G4PVPlacement(0, G4ThreeVector(0.,-offset,10.5*cm), 
                        logicTeflonY, "physTeflonYminus", logicWorld, false, 0, checkOverlaps);
    
    // Y yüzeylerine optik yüzey tanımla
    new G4LogicalBorderSurface("TeflonYplus_Surface", physDetector, physTeflonYplus, reflectiveSurface);
    new G4LogicalBorderSurface("TeflonYminus_Surface", physDetector, physTeflonYminus, reflectiveSurface);


    // ================================
    // Z+ plakası (delik yok - tam yansıtıcı)
    // ================================
    G4Box *solidTeflonZ = new G4Box("solidTeflonZ", 
        0.5 * detectorSize, 
        0.5 * detectorSize, 
        0.5 * teflonThickness);
    G4LogicalVolume *logicTeflonZ = new G4LogicalVolume(solidTeflonZ, teflonMat, "logicTeflonZ");
    logicTeflonZ->SetVisAttributes(teflonVisAtt);
    
    auto physTeflonZplus = new G4PVPlacement(0, G4ThreeVector(0.,0.,10.5*cm + offset), 
    logicTeflonZ, "physTeflonZplus", logicWorld, false, 0, checkOverlaps);

    // Z+ yüzeyine optik yüzey tanımla
    new G4LogicalBorderSurface("TeflonZplus_Surface", physDetector, physTeflonZplus, reflectiveSurface);


    // ==========================
    // Z− plakası (delikli)
    // ==========================
    G4Box *solidTeflonZminus = new G4Box("solidTeflonZminus", 
                                            0.5 * detectorSize, 
                                            0.5 * detectorSize, 
                                            0.5 * teflonThickness);

    // Delik: 1 cm çapında, plaka kalınlığını tamamen kesecek kadar uzun
    G4Tubs* holeZminus = new G4Tubs("holeZminus", 
                                    0.*cm, 
                                    0.5*cm, 
                                    0.5*teflonThickness + 0.001*cm, 
                                    0.*deg, 
                                    360.*deg);

    // Çıkarma işlemi: delik plakanın kendi local merkezinden açılıyor
    G4SubtractionSolid* solidTeflonZminusHole = new G4SubtractionSolid("solidTeflonZminusHole",
                                                solidTeflonZminus,
                                                holeZminus,
                                                0,
                                                G4ThreeVector(0.,0.,0.));

    // Logical volume
    G4LogicalVolume *logicTeflonZminus = new G4LogicalVolume(solidTeflonZminusHole, teflonMat, "logicTeflonZminus");
    logicTeflonZminus->SetVisAttributes(teflonVisAtt);

    // Yerleştirme: scintilatöre bakan Z− plakası
    auto physTeflonZminus = new G4PVPlacement(0, G4ThreeVector(0.,0.,10.5*cm - offset), 
                        logicTeflonZminus, 
                        "physTeflonZminus", 
                        logicWorld, 
                        false, 0,
                        checkOverlaps);
    
    // Z- yüzeyine optik yüzey tanımla (sintilatöre bakan yüzey - sadece delik olmayan kısım)
    new G4LogicalBorderSurface("TeflonZminus_Surface", physDetector, physTeflonZminus, reflectiveSurface);
    
    // ========================
    // Air Hole (Z−)
    // ========================
    G4Tubs* airHoleZSolid = new G4Tubs("airHoleZSolid", 
                                        0.*cm, 
                                        0.5*cm, 
                                        0.5*teflonThickness + 0.001*cm, 
                                        0.*deg, 
                                        360.*deg);

    G4ThreeVector posAirHoleZ = G4ThreeVector(0., 0., 10.5*cm - offset);  // Z− plakası ile aynı konum

    G4LogicalVolume* logicAirHoleZ = new G4LogicalVolume(airHoleZSolid, airMat, "logicAirHoleZ");

    new G4PVPlacement(0,    // rotasyon yok
                    posAirHoleZ, 
                    logicAirHoleZ, 
                    "physAirHoleZ", 
                    logicWorld, 
                    false, 
                    0, 
                    checkOverlaps);
    
        
    // ================================
    // Alüminyum Dedektör (-X deliğinin önüne)
    // ================================
    G4NistManager* nist = G4NistManager::Instance();
    G4Material* aluminumMat = nist->FindOrBuildMaterial("G4_Al");

    G4double alThickness = 0.01 * cm; // 0.1 mm
    G4double alSize = 2.0 * cm;       // 2x2 cm

    G4Box* solidAlDetector = new G4Box("solidAlDetector",
                                    0.5 * alThickness,
                                    0.5 * alSize,
                                    0.5 * alSize);

    // 🔹 Normal alüminyum MPT (yansıtıcı değil)
    G4MaterialPropertiesTable* aluminumMPT = new G4MaterialPropertiesTable();

    G4double alPhotonEnergy[] = {2.034*eV, 2.068*eV, 2.103*eV, 2.139*eV, 2.755*eV, 4.136*eV};
    const G4int alNEntries = sizeof(alPhotonEnergy)/sizeof(G4double);

    // Kırılma indisi
    G4double alRindex[] = {1.44, 1.44, 1.44, 1.44, 1.44, 1.44};
    // Absorpsiyon uzunluğu
    G4double alAbsorption[] = {0.001*mm, 0.001*mm, 0.001*mm, 0.001*mm, 0.001*mm, 0.001*mm};

    aluminumMPT->AddProperty("RINDEX", alPhotonEnergy, alRindex, alNEntries);
    aluminumMPT->AddProperty("ABSLENGTH", alPhotonEnergy, alAbsorption, alNEntries);

    aluminumMat->SetMaterialPropertiesTable(aluminumMPT);

    // Mantıksal hacim
    logicAlDetector = new G4LogicalVolume(solidAlDetector, aluminumMat, "logicAlDetector");

    // Pozisyon (senin formülün)
    G4double alPosition = -offset - 0.5*teflonThickness - 2*mm - 0.5*alThickness;

    // Yerleştirme
    auto physAlDetector = new G4PVPlacement(0,
                                            G4ThreeVector(alPosition, 0., 10.5*cm),
                                            logicAlDetector,
                                            "physAlDetector",
                                            logicWorld,
                                            false,
                                            0,
                                            checkOverlaps);

    // Görselleştirme
    auto alVisAtt = new G4VisAttributes(G4Color(0.7, 0.7, 0.7, 0.8)); // gri
    alVisAtt->SetForceSolid(true);
    logicAlDetector->SetVisAttributes(alVisAtt);

    // 🔹 Optical Surface: detection efficiency
    G4double alReflectivity[alNEntries];
    G4double alEfficiency[alNEntries];
    for (int i = 0; i < alNEntries; i++) {
        alReflectivity[i] = 0.0; // yansıma yok
        alEfficiency[i]   = 1.0; // %100 tespit
    }

    G4MaterialPropertiesTable* alSurfaceMPT = new G4MaterialPropertiesTable();
    alSurfaceMPT->AddProperty("REFLECTIVITY", alPhotonEnergy, alReflectivity, alNEntries);
    alSurfaceMPT->AddProperty("EFFICIENCY",   alPhotonEnergy, alEfficiency,   alNEntries);

    auto alSurface = new G4OpticalSurface("AluminumDetectorSurface");
    alSurface->SetType(dielectric_metal);
    alSurface->SetFinish(polished);
    alSurface->SetModel(unified);
    alSurface->SetMaterialPropertiesTable(alSurfaceMPT);

    // World → Alüminyum sınırına uygula
    new G4LogicalBorderSurface("AlSurface",
                                physAirHoleX,  // deliğin olduğu hava hacmi
                                physAlDetector,
                                alSurface);

    G4cout << "🔧 Aluminum detector (NON-REFLECTIVE, DETECTS OPTICAL PHOTONS) created at X position: "
        << alPosition/mm << " mm" << G4endl;

    return physWorld;}
        

void PMDetectorConstruction::ConstructSDandField()
{
    if (logicDetector) {
        auto* scintSD = new PMSensitiveDetector("ScintillatorSD");
        logicDetector->SetSensitiveDetector(scintSD);
        G4SDManager::GetSDMpointer()->AddNewDetector(scintSD);
    }

    if (logicAlDetector) {
        auto* alSD = new PMSensitiveDetector("AluminumSD");
        logicAlDetector->SetSensitiveDetector(alSD);
        G4SDManager::GetSDMpointer()->AddNewDetector(alSD);
    }
}
    