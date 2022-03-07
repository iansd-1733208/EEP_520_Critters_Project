#ifndef __SPINNER_CRITTER__H
#define __SPINNER_CRITTER__H

#include "enviro.h"
#include "critter.h"

using namespace enviro;

// The Spinner's Normal State
class SpinnerNormalState : public CritterNormalState {
public:
    // Just spin, as the critter name suggests
    void action() {
        track_velocity(0.0, 5.0);
    }
};

// The Spinner's Ally State
class SpinnerAllyState : public CritterAllyState {
public:
    // Same as Normal State
    void action() {
        track_velocity(0.0, 5.0);
    }
};

// The Spinner's Wall State
class SpinnerWallState : public CritterWallState {
public:
    // Back away from walls and try to stop spinning
    void action() {
        track_velocity(-5.0, -5.0);
    }
};

// The Spinner's Attack State
class SpinnerAttackState : public CritterAttackState {
public:
    // Zoom toward the other Critter, and spin quickly.
    void action() {
        track_velocity(10.0, 20.0);
    }
};

// The Spinner's Controller
// Manages all the states for the spinner
class SpinnerController : public CritterController {
public:
    SpinnerController() : CritterController("SpinnerController") {
        attachStates(m_SpinnerNormal, m_SpinnerAlly, m_SpinnerWall, m_SpinnerAttack);
    }

private:
    SpinnerNormalState m_SpinnerNormal;
    SpinnerAllyState m_SpinnerAlly;
    SpinnerWallState m_SpinnerWall;
    SpinnerAttackState m_SpinnerAttack;
};

// The Spinner Critter itself
// Just needs to install the controller at this point.
class Spinner : public Critter {
public:
    Spinner(json spec, World& world) : Critter(spec, world) {
        installController(c);
    }

private:
    SpinnerController c;
};

DECLARE_INTERFACE(Spinner)

#endif // __SPINNER_CRITTER__H