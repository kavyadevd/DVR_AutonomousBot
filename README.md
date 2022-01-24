# DVR_AutonomousBot
Arduino code for autonomus robot navigating through a controlled pre-described path using direct vector routing (for navigation) and PID algorithm (for line following)
Code uploaded on Teensy development board (ARM Cortex-M7 at 600 MHz processor).



https://user-images.githubusercontent.com/13993518/150846696-9bbe2c9b-5548-45b2-9662-19cdeda09f50.mp4



__________________________________
Task:
* The robot game field is divided into 3 areas, consisting of Start Zone, Loading Zone, Throwing Zone, Manual Robot Area, and Automatic Robot Area.The throwing Area is placed with * Ring Trees, Normal Rings, Golden Ring, and Golden Cup.Before a game starts, ten (10) Normal Shuttlecocks and five (5) Golden Shuttlecocks are placed inside Loading Zone.
* When a game starts, Manual Robot will pick Normal Shuttlecocks and handle it to Automatic Robot.After receiving the Normal Shuttlecock, the Automatic Robot will move into TZ1, TZ2 and throw the shuttlecock at the Normal Ring. 
* If the shuttlecock goes through the ring successfully, points will be given.Manual Robot can go and pick up the Golden Shuttlecock only after at least one Normal Shuttlecock thrown from each TZ1 and TZ2 went through the Normal Ring successfully.
* After receiving Golden Shuttlecock from Manual Robot, Automatic Robot can move to TZ3 and throw the Golden Shuttlecock at the Golden Ring. If the Golden Shuttlecock goes through the Golden Ring successfully, points will be given. 
(^ rule source ABU Robocon 2018 rule book)

### The repository is of the code for automatic robot successfully completing the task described above



