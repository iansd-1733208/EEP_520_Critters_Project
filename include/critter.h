#ifndef __CRITTER_H
#define __CRITTER_H

#include "enviro.h"
#include <string>
#include <math.h>
#include <vector>

using namespace enviro;

enum CritterStateType {
    CS_DEAD,
    CS_NORMAL,
    CS_ALLY,
    CS_WALL,
    CS_ATTACK,
    CS_NUM_STATES
};
static const std::string CritterStateStrings[] = { "DeadState", "NormalState", "AllyState", "WallState", "AttackState" };

// Base Class for all Critter States
// Automatically calculates what a critter is looking at
// and emits a signal if the critter needs to change states.
class CritterState : public State, public AgentInterface {
friend class CritterController;

public:
    // CritterState::action: meant to be overwritten by each User-Defines state.
    virtual void action() {
        damp_movement();
    }

    // Called while the process is running
    // Calculates what the Critter sees and
    // changes states is necessary.
    void during() {
        if (typeOfState() != CS_DEAD) {
            action();
            double distance = sensor_value(0);
            if ( m_viewDistance != 0 && distance <= m_viewDistance ) {
                std::string otherCritter = isLookingAt();
                if (otherCritter == "StaticObject" || otherCritter == "dead_critter") {
                    if (typeOfState() != CS_WALL) emit(Event(m_tickName + "_" + CritterStateStrings[CS_WALL]));
                } else if (otherCritter == agent->name()) {
                    if (typeOfState() != CS_ALLY) emit(Event(m_tickName + "_" + CritterStateStrings[CS_ALLY]));
                } else if (otherCritter != "None") { 
                    if (typeOfState() != CS_ATTACK) emit(Event(m_tickName + "_" + CritterStateStrings[CS_ATTACK]));
                } else if (typeOfState() != CS_NORMAL) {
                    emit(Event(m_tickName + "_" + CritterStateStrings[CS_NORMAL]));
                }
            } else if (typeOfState() != CS_NORMAL) {
                emit(Event(m_tickName + "_" + CritterStateStrings[CS_NORMAL]));
            }
        } else {
            damp_movement();
        }
    }

    // Needed by State base class.
    void entry(const Event& e) {}
    void exit(const Event& e) {}

    // Returns the CritterStateType corresponding to the type of state.
    virtual CritterStateType typeOfState() const {
        return CS_DEAD;
    }

protected:
    double m_viewDistance;
    std::string m_tickName;

    // Returns a cpVector that respresents
    // the given cpVect rotated by theta (radians).
    cpVect rotate(cpVect v, cpFloat theta) {
        return { x: cos(theta) * v.x - sin(theta) * v.y,
                y: sin(theta) * v.x + cos(theta) * v.y };
    };

    // TODO: find another way.
    // Agents sensor configuration is not available to us
    // so we have to recreate the calculation to detect
    // what the agent is looking at.
    // We assume the sensor is at X, Y, Theta = 0, 0, 0.
    std::string isLookingAt() {
        double distance = 10000;
        World * world = agent->get_world_ptr();

        double theta = agent->angle();
        cpVect P = agent->position();

        // Range Sensor must be configured to X: 0, Y: 0, for now
        cpVect range_location = {x: 0, y: 0};
        cpVect Lrot = rotate(range_location, theta);
        cpVect start =  cpvadd(P, Lrot);

        // Range Sensor must have theta = 0, for now
        cpFloat angle = 0 + agent->angle();              
        cpVect end = cpvadd(start, { x: 1000 * cos(angle), y: 1000 * sin(angle)});       

        Agent* other_critter = nullptr;
        world->all([this, &distance, &other_critter, &start, &angle, &end](Agent& other) {
            if ( &other != agent ) {
                cpSegmentQueryInfo info;        
                cpShape * shape = other.get_shape();
                cpShapeSegmentQuery(shape, start, end, 0, &info);

                if ( info.shape != NULL ) {
                    double d = cpvdist(start, info.point);
                    if ( d < distance ) {
                        distance = d;
                        other_critter = &other;
                    }
                }
            }
            
        });
        if (other_critter != nullptr ) {
            // Kill the critter if withing 10 units of another critter.
            if (distance < 10 && other_critter->name() != "StaticObject" && other_critter->name() != agent->name() && other_critter->name() != "dead_critter") {
                emit(Event(m_tickName + "_" + CritterStateStrings[CS_DEAD]));
                agent->set_name("dead_critter");
                return "None";
            }
            return other_critter->name();
        } else {
            return "None";
        }

    }

private:
    // Sets the view distance of the critter.
    void setViewDistance(double distance) {
        m_viewDistance = distance;
    }

    // Sets the tick name for the critter states.
    void setTickName(std::string tickName) {
        m_tickName = tickName;
    }
};

// Base class for all Critter Normal States.
// Derived classes should define void action()
class CritterNormalState : public CritterState {
public:
    CritterStateType typeOfState() const {
        return CS_NORMAL;
    }
};

// Base class for all Critter Ally States.
// Derived classes should define void action()
class CritterAllyState : public CritterState {
public:
    CritterStateType typeOfState() const {
        return CS_ALLY;
    }
};

// Base class for all Critter Wall States.
// Derived classes should define void action()
class CritterWallState : public CritterState {
public:
    CritterStateType typeOfState() const {
        return CS_WALL;
    }
};

// Base class for all Critter Attack States.
// Derived classes should define void action()
class CritterAttackState : public CritterState {
public:
    CritterStateType typeOfState() const {
        return CS_ATTACK;
    }
};

// Base class for all Critter Controllers
// All derived classes must call 
// attackStates(normalState, allyState, wallState, attackState) in its constructor.
class CritterController : public StateMachine, public AgentInterface {
public:
    CritterController(std::string stateMachineName="CritterController") : StateMachine(stateMachineName) {
        m_tickName = "tick_" + std::to_string(rand() % 1000);
        m_states = {};        
    }

    // Attaches the 4 states to the Critter and configures the transitions between them.
    void attachStates(CritterNormalState& normal, CritterAllyState& ally, CritterWallState& wall, CritterAttackState& attack) {
        m_states.push_back(&normal);
        m_states.push_back(&ally);
        m_states.push_back(&wall);
        m_states.push_back(&attack);
        set_initial(normal);
        for (CritterState* state_ptr : m_states) {
            switch(state_ptr->typeOfState()) {
                case CS_NORMAL:
                    add_transition(m_tickName + "_" + CritterStateStrings[CS_ALLY], normal, ally);
                    add_transition(m_tickName + "_" + CritterStateStrings[CS_WALL], normal, wall);
                    add_transition(m_tickName + "_" + CritterStateStrings[CS_ATTACK], normal, attack);
                    add_transition(m_tickName + "_" + CritterStateStrings[CS_DEAD], normal, m_deadState);
                    break;
                case CS_ALLY:
                    add_transition(m_tickName + "_" + CritterStateStrings[CS_NORMAL], ally, normal);
                    add_transition(m_tickName + "_" + CritterStateStrings[CS_WALL], ally, wall);
                    add_transition(m_tickName + "_" + CritterStateStrings[CS_ATTACK], ally, attack);
                    add_transition(m_tickName + "_" + CritterStateStrings[CS_DEAD], ally, m_deadState);

                    break;
                case CS_WALL:
                    add_transition(m_tickName + "_" + CritterStateStrings[CS_NORMAL], wall, normal);
                    add_transition(m_tickName + "_" + CritterStateStrings[CS_ALLY], wall, ally);
                    add_transition(m_tickName + "_" + CritterStateStrings[CS_ATTACK], wall, attack);
                    add_transition(m_tickName + "_" + CritterStateStrings[CS_DEAD], wall, m_deadState);
                    break;
                case CS_ATTACK:
                    add_transition(m_tickName + "_" + CritterStateStrings[CS_NORMAL], attack, normal);
                    add_transition(m_tickName + "_" + CritterStateStrings[CS_ALLY], attack, ally);
                    add_transition(m_tickName + "_" + CritterStateStrings[CS_WALL], attack, wall);
                    add_transition(m_tickName + "_" + CritterStateStrings[CS_DEAD], attack, m_deadState);
                    break;
                default:
                    throw std::invalid_argument("Illegal State Array order { NormalState, AllyState, WallState, AttackState } but found " + CritterStateStrings[state_ptr->typeOfState()]);
            }
            state_ptr->setTickName(m_tickName);
        }
    }

    // Sets the view distance of the critter.
    void setViewDistance(double distance) {
        for (CritterState* state_ptr : m_states) {
            state_ptr->setViewDistance(distance);
        }
    }

protected:
    std::string m_tickName;
    std::vector<CritterState*> m_states;
    CritterState m_deadState;
};

// Base class for all Critters
// Derived classes must call
// installController(controller) in its constructor.
class Critter : public Agent {
public:
    Critter(json spec, World& world) : Agent(spec, world) {
        m_c = nullptr;
        m_viewDistance = spec["definition"]["view"].get<cpFloat>();
    }

    // Sets the controller for the Critter.
    void installController(CritterController& c) {
        m_c = &c;
        m_c->setViewDistance(m_viewDistance);
        add_process(*m_c);
    }

private:
    CritterController* m_c;
    double m_viewDistance;
};

#endif // __CRITTER_H