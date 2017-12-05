# Elevator
A multi-elevator simulation program.

This program simulates the dispatch of the task on the multi-elevator system on Win32 platform and take some factors such as doors and safety into consideration.

## How to configure config.ini
*Please do **not** include the annotation  in the configuration file.*


```
Num=3         #The number of elevators
MaxFloor=9    #The number of floors of a building
RunSpeed=1800 #Time consumption when the elevator moves one floor, in milliseconds
WaitTime=3000 #Idle time before closing the door, in milliseconds
DoorTime=1000 #Time consumption for the door opening and closing, in milliseconds
```
