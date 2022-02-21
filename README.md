# Final Embedded Systems Project
## Objectives
The goal of this project was to design a traffic light control system. The system contains simply two traffic lights. One allows cars to move from north to south, and the other one allows cars to move from east to west. Beside each traffic there is also a pedestrian traffic light. The pedestrian has to press on a button to have his light green to cross the street safely. </br>
By the end of this project I mastered the following:
1. Timers: Used two timers to configure the span of the traffic lights. </br>
2. GPIO: Used LEDs as outputs, and push buttons as input. </br>
3. Interrupts: Used interrupts for the push buttons and timers </br>

## Features
### Set The Normal Cars Traffic Lights </br>
Used one of the timers to have the two car traffic lights working. The traffic light shall stay GREEN for 5 seconds, then YELLOW for 2 seconds, then turns RED. When one of the traffic lights is set to RED the other one has to go GREEN exactly after 1 second. The same sequence then is repeated again. </br>
GREEN Light for 5 seconds Then YELLOW Light for 2 seconds Then RED. </br>
Then Wait 1 second, then start the sequence on the other traffic.</br>

### Implement the pedestrian Traffic Lights </br>
Used two push buttons and 4 LEDs for this system. There is two pedestrian traffics, each with a push button and 2 LEDS: GREEN and RED. Whenever a pedestrian presses the button, the traffic light that is green shall be interrupted, and the pedestrian traffic light will be green for 2 seconds. Then it will be back to RED and the traffic light that was interrupted will resume from when it was paused.
