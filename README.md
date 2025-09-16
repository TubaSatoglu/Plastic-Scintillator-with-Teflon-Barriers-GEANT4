# Plastic-Scintillator-with-Teflon-Barriers-GEANT4

This project implements a Geant4-based simulation of a **plastic scintillator wrapped with Teflon barriers**.  
Focuses on optical photon yield and boundary reflections.  

---

## Geometry Definition
Implemented in **PMDetectorConstruction.cc/hh**.  
- Builds a **plastic scintillator volume**.  
- Surrounds it with **Teflon reflective barriers**.  
- Assigns optical properties to both materials.  

---

## Physics List
Defined in **PMPhysicsList.cc/hh**.  
- FTFP_BERT_HP physics list.  
- Adds optical photon processes.  

---

## Action Initialization
Defined in **PMActionInitialization.cc/hh**.  
- Registers PMRunAction, PMEventAction, PMSteppingAction, PMSensitiveDetector.  

---

## Primary Generator Action
Defined in **PMPrimaryGenerator.cc/hh**.  
- Generates alpha or gamma particles into the scintillator.  
- Configurable particle type, energy, and angle.  

---

## ▶️ Build Instructions

```bash
mkdir build && cd build
cmake ..
make
./sim run.mac
