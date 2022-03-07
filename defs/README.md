Put agent definitions in this folder.<br>
Example:

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
        "location": { "x": 0, "y": 0 },
        "direction": 0 // in radians
      }
    ],
    "view": 100, // How far the critter can see. 
    "mass": 1,    
    "controller": "lib/crittername.so"
    }