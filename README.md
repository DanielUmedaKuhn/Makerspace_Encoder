Makerspace 3D Printer Fail Identification Project
======

### This project aims to track and identify feeding-related fails in 3D printers. It has been tested and implemented in Ultimaker S3, S5 and S7 models.

Parts utilized:
* Arduino nano ESP32
* TCRT5000 Optic Encoder
* 220 Ohm resistor
* LED diode 2.25V
* 3D printed encoder support and grooved disc 

The encoder is fixed to the support and positioned in front of the disc attached to the filament roll. The system identifies when the printer is not being fed during a print through inactivity in the filament roll. There is a warning light to let the user know when there is a problem with the feeding mechanism.

Author:
[Daniel Umeda Kuhn](https://github.com/DanielUmedaKuhn)
