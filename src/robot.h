#ifndef __ROBOT_CRITTER__H
#define __ROBOT_CRITTER__H 

#include "enviro.h"
#include "critter.h"

using namespace enviro;

// The Robot's Normal State
class RobotNormalState : public CritterNormalState {
public:
    // Randomly turn left or right, while moving forward
    void action() {
        if (rand() % 4 == 0) {
            track_velocity(150.0, -3.0);
        } else {
            track_velocity(150.0, 3.0);
        }
    }
};

// The Robot's Ally State
class RobotAllyState : public CritterAllyState {
public:
    // Back away from Allies
    void action() {
        track_velocity(-10.0, -1.0);
    }
};

// The Robot's Wall State
class RobotWallState : public CritterWallState {
public:
    // Back away from walls.
    void action() {
        track_velocity(-10.0, 5.0);
    }
};

// The Robot's Attack State
class RobotAttackState : public CritterAttackState {
public:
    // Spin in hopes that the opposing critter dies.
    void action() {
        track_velocity(0.0, 150.0);
    }
};

// The Robot's Controller
// Manages all the states for the robot
class RobotController : public CritterController {
public:
    RobotController() : CritterController("RobotController") {
        attachStates(m_robotNormal, m_robotAlly, m_robotWall, m_robotAttack);
    }

private:
    RobotNormalState m_robotNormal;
    RobotAllyState m_robotAlly;
    RobotWallState m_robotWall;
    RobotAttackState m_robotAttack;
};

// The Robot Critter itself
// Just needs to install the controller at this point.
class Robot : public Critter {
public:
    Robot(json spec, World& world) : Critter(spec, world) {
        installController(c);
    }

private:
    RobotController c;
};

DECLARE_INTERFACE(Robot)

#endif // __ROBOT_CRITTER__H 