# EEP 520 Winter 2022 Project

Software Engineering for Embedded Applications<br>
Developed by Ian Davidson

## Overview
The goal of this project is to use Elma and Enviro to create a base for a User-Defined "Critters" to battle. Critters respond to what they see in the world, with the ultimate goal to be the last species alive. Critters attack other species, but be careful, critters die if they run directly into a Critter that is not their species. This creates interesting strategies for your critter.

## Usage
### Install
Clone this repository into any directory.

The easiest way to install the dependencies and run this project is with [Docker](https://www.docker.com/).<br>

Otherwise, isntall all the dependencies using the instructions in the respositories mentioned [below](#repostories-used).<br>


### Make your own Critter
See **src/** for examples of critter examples.<br>
Each critter is made up of 3 components:
1. [Header File](#header) (**src/robot.h**): Defines the critter, states, and controller.
2. [CC File](#cc-file) (**src/robot.cc**): Typically empty, used by the Makerfile to produce the controller.
3. [Definition](#definition) (**defs/robot.json**): Used to define the physical dimensions of the critter.

#### Header
The meat of this file defines the functionality of each state of the critter.

##### States
Each critter has 4 definable state classes: **Normal**, **Ally**, **Wall**, **Attack**.<br>
| State  | Trigger                                  | Inherits from      |
|--------|------------------------------------------|--------------------|
| Normal | Initial<br> Does not see anything            | CritterNormalState |
| Ally   | Sees a Live Critter of the same type     | CritterAllyState   |
| Wall   | Sees any StaticObject<br> Sees a Dead Critter | CritterWallState   |
| Attack | Sees a Live Critter of a different type  | CritterAttackState |

Each of these classes allow each critter to override the method **void action()**. Which is called when the critter is in the corresponding state. Any sequence of the following methods are valid:

    apply_force(cpFloat thrust, cpFloat torque);
    track_velocity(cpFloat linear_velocity, cpFloat angular_velocity);
    damp_movement(); // track_velocity(0, 0)
    move_toward(cpFloat x, cpFloat y);
    teleport(cpFloat x, cpFloat y, cpFloat theta);

##### Controller
This class must inherit from **CritterController** and have 4 private members, one for each of the states defined above. The constructor must be in the following format:

    class YourCritterController : public CritterController {
    public:
        YourCritterController() : CritterController("YourCritterController") {
            attachStates(m_YourNormalState, m_YourAllyState, m_YourWallState, m_YourAttackState);
        }

    private:
        YourCritterNormalState m_YourNormalState;
        YourCritterAllyState m_YourNormalState;
        YourCritterWallState m_YourNormalState;
        YourCritterAttackState m_YourNormalState;
    }

##### Critter
This class installs the controller created above the the critter. The controller is a private member and installed in the constructor.

    class YourCritter : public Critter {
    public:
        YourCritter(json spec, World& world) : Critter(spec, world) {
            installController(c);
        }

    private:
        YourCritterController c;
    }

#### CC File
This file is used by the make file to create the full controller. Typically the following lines are used.

    #include "you_header.h"

    using namespace enviro;

#### Definition
This file defines the physical attributes of your critter. This files should be places in **defs/yourcritter.json** and in the format of a JSON file. An example of this format can be seen below. It is important to note that the "front" of the critter is defined as the segment between the first and second verticies in the **shape** array. Due to some limitation of Enviro, the first sensor in your critter must be a range sensor at location **{ "x": 0, "y": 0 }** and direction **0**. The **view** attribute defines how far the critter can see, if this value is 0, your critter is blind. Do not make a blind critter.

    {
    "name": "Critter Name",
    "type": "dynamic",
    "description": "A Cool Critter",
    "shape": [
        { "x": -20, "y": -20 }, // 'Front' is between
        { "x":   0, "y": -10 }, // these two verticies.
        { "x":   0, "y":  10 },
        { "x": -20, "y":  20 }
    ],
    "friction": {
      "collision": 2,
      "linear": 40,
      "rotational": 600
    },
    "sensors": [
      {
        "type": "range",
        "location": { "x": 0, "y": 0 }, // Must be 0, 0.
        "direction": 0 // in radians, must be 0.
      }
    ],
    "view": 100, // How far the critter can see, should not be 0.
    "mass": 1,    
    "controller": "lib/crittername.so"
    }

### Run
If you are using docker (recommended), run the following command from the repository. 

    docker run -p80:80 -p8765:8765 -v $PWD:/source -it klavins/enviro:alpha bash

To start the server run:

    esm start

To create the controller (lib/crittername.so) files run make from the repository.

    make

Add your agents, and their spawning coordinates, to **config.json** using the example below.

    "agents": [
        {
            "definition": "defs/yourcritter.json",
            "style": { "fill": "blue", "stroke": "black" },
            "position": { // (0, 0) is the middle of the screen, PosX is RIGHT, PosY is DOWN
                "x": 0, 
                "y": 0, 
                "theta": 0 // in Radians
            }
        }
    ]

And finally, to view your critters interacts navigate to <http://localhost> and run:

    enviro

## Challenges
Originally, the plan was for critters to attack other critters and either remove the other cirtter from the board or convert them into 'allies' in which they mimic the actions of the attacker. However, and Enviro's World instance does not support removing agents from the World, nor conversion between two Agents. The critters now die if they 'see' another critter comes within 10 units of them. This changes the strategy for the critters, since they have to approach the other critters, but must turn away in hopes that the other critter faces them within 10 units.<br>

Enviro does not natively support a query to get what is on the other end of range sensors. To remedy this, the critters range sensor MUST be configured to (X, Y, Theta) -> (0, 0, 0) as this is used to recreat the range sensor data and capture what the Critter is looking at. Since an agents sensor configuration is not accessible, this assumption must be made.<br>

## Inspiration
The inspiration behind this project is loosley based on University of Washington CSE 142's final assignment, Critters. The specification of this project can be found [here](https://courses.cs.washington.edu/courses/cse142/22wi/assessments/a8.html).<br>


## Repostories Used
Thank you to Professor Eric Klavins for the creation and continued development of Elma and Enviro.<br>
[Enviro](https://github.com/klavinslab/enviro)<br>
[Elma](https://github.com/klavinslab/elma)

