# AquariumDaglig
This is my project for a switched solar day light. Unfortunatly I blew up my remaining LEDs so the firmware has stalled at this working version.
![layout](https://user-images.githubusercontent.com/25556420/138914806-60dadae5-05cb-48ce-9aa2-079440f9cf45.png)
## Theory of Operation
TCB0 is configured in single-shot mode, the start of which is triggered by AC0's output via the event system.
When TCB0 is triggered PA6 is driven low, turning off the MOSFET allowing the inductor current to circulate
through the schottky diode and the LED string; resetting the inductor. In the main loop the PV voltage is monitored
based on it's value the reference for AC0 is either increased or decreased setting the peak inductor current.


