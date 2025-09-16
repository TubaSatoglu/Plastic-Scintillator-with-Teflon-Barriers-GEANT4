#ifndef PMSTEPPINGACTION_HH
#define PMSTEPPINGACTION_HH

#include "G4UserSteppingAction.hh"

class PMEventAction;

class PMSteppingAction : public G4UserSteppingAction {
public:
    // Constructor now takes a pointer to the event action
    PMSteppingAction(PMEventAction* eventAction);
    virtual ~PMSteppingAction();

    virtual void UserSteppingAction(const G4Step* step) override;

private:
    PMEventAction* fEventAction;
};

#endif
