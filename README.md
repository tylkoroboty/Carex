# Welcome in CareX Project

It is device to collect data from my Opel Meriva A 2003 1.6

## What is done?
### Communication with LCD using ILI9341 controller
### Classes to draw:
* Pixel
* Line
* Rectangular

### RN42 Module Control class:
(HC-05 changed due to link ELM327 issue)
* Connect - working
* Send - working
* Receive - working
* Reset - working
* IsConnected - working

### OBD diagnostic class:
* GetRPM - working
* GetSpeed - not working yet
* Is Active - checking answer for AT command

## What is yet to come?
### Communication with ELM327 bluetooth module [done]
### Drawing RPM gauge [done]

### Drawing Speed gauge
### Drawing Oil Temperature gauge
### Navigate with tauch panel
### Drawing car parameter as diagrams
### Calculation of fuel consumption
### Estimation of driving range

### Calculating gear shifts for best performance
### Control inner and outter light strips
