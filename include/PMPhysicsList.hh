#ifndef PMPHYSICSLIST_HH
#define PMPHYSICSLIST_HH

#include "G4VModularPhysicsList.hh"

class PMPhysicsList : public G4VModularPhysicsList {
public:
    PMPhysicsList();
    virtual ~PMPhysicsList();

    // Override functions correctly
    void ConstructParticle() override;
    void ConstructProcess() override;
    void SetCuts() override;

    // No override here because these are unique to this class
    void DefineParticles();
    void DefineCuts();
};

#endif
