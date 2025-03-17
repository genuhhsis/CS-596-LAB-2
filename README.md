# CS:596 LAB 2
 * Genesis Anne Villar (RED ID: 824435476)
 * Steven Gervacio (RedID: 825656527)
 * CS 596 IOT - Prof. Donyanavard
 * Due Date: 3/17/2025
# Highway Traffic Light Controller with ESP32
This project implements a simplified traffic light controller using an ESP32 microcontroller. The system is designed to simulate a pedestrian crossing at a highway, featuring state transitions, timing controls, and accessibility features for visually impaired pedestrians. In addition, the display of the TTGOLily ESP32 displays what state and transition we are in for easy visibility and debugging.

# Top Down View of Circuit
- PINS
Green LED = 12
Yellow LED = 15
Red LED = 2
- Buzzer = 21
- Button = 22
- 220 OHM resistor was attached to LED + speaker for current limiting
![Top-down view of traffic light circuit](Top%20down%20view%20of%20circuit%20-%20villar%20and%20gervacio.jpg)

# Features 
Complete traffic light cycle (Red → Red-Yellow → Green → Yellow → Red)
Pedestrian-activated crossing request via touch button
Audio feedback system with different patterns for each traffic light state
Configurable timing parameters for all states
Safety features including minimum green time after button press

# Start Up Sequence
- System Initializes in Red State
--- Red LED turns on
--- Buzzer starts pulsing at 250ms on, 250ms off pattern (for visually impaired aid)
--- System remains in this state for 10 seconds


- Transition to Red-Yellow State
--- Both Red and Yellow LEDs turn on
--- Buzzer turns off during this transition state
--- System remains in this state for 2 seconds


- Transition to Green State
--- Red and Yellow LEDs turn off, Green LED turns on
--- Buzzer changes to slower pattern: 500ms on, 1500ms off
--- System remains in this state indefinitely until the pedestrian button is pressed

# Button Press Sequence

- Pedestrian Presses Button During Green State
--- System acknowledges button press but continues in Green state
--- System must remain in Green for at least 5 seconds after button press
--- Buzzer continues with 500ms on, 1500ms off pattern


-Transition to Yellow State (after 5+ seconds from button press)
--- Green LED turns off, Yellow LED turns on
--- Buzzer turns off during this transition state
--- System remains in this state for 2 seconds

- Transition to Red State
--- Yellow LED turns off, Red LED turns on
--- Buzzer resumes rapid pulsing at 250ms on, 250ms off pattern
--- System remains in this state for 10 seconds


- Transition to Red-Yellow State
--- Both Red and Yellow LEDs turn on
--- Buzzer turns off during this transition state
--- System remains in this state for 2 seconds


- Return to Green State
--- Red and Yellow LEDs turn off, Green LED turns on
--- Buzzer returns to slower pattern: 500ms on, 1500ms off
--- System remains in this state until the next button press

# Buzzer Operation Summary

Active During Red State: Fast pulsing (250ms on, 250ms off)
Active During Green State: Slow pulsing (500ms on, 1500ms off)
Inactive During Yellow and Red-Yellow States: Buzzer is off

This cycle continues indefinitely, with the system staying in Green until a pedestrian requests to cross by pressing the button.
