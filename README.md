# Halloween Zipline
## Basic concept
There are two light barriers along a corridor. At the end of the corridor, a puppet is suspended from the ceiling on a timing belt, driven by a servo motor. Once a person passes the first light barrier, the puppet gets accelerated towards the person. At the same time, two 230 V outlets get turned on to light up the puppet. An MP3 module plays a sound effect through speakers. To avoid the person getting hit by the puppet, the motor stops immediately  once the person passes the second light barrier. The puppet remains at its end point for a few seconds, upon which it returns to its resting position. The effects can also be launched manually using a hand controller.

## Operation
1. Plug in all sensors and outputs into the labeled connectors.
2. Insert a micro-SD card into the MP3 module. The mp3 file should be placed into a folder named '1'.
3. Turn on power. The system boots into the emergency state. Press and release the emergency stop button once to get to normal operation.

## Components
- Microcontroller: Arduino Mega
- Motor: iSV57T-180S
- Inductive endstop sensor: LJ12A3-4-Z/BX
- Light barrier: E3JK-R4M1
- MP3 module: YX5300
- 4 channel relais card
- 36V 9.7A power supply
- 5V power supply
- GT2 timing belt and pulley
