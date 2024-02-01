# Alko - a vending machine project

## Description
This is some old code for a defunct vending machine project. A used vending machine, maybe a DNCB 414CC, was integrated with an internal customer system.

## High level system description
A Raspberry Pi is communicating with an internal customer system via HTTP, and is communicating with an Arduino via serial over USB. A magnetic card reader (MagTek) is connected to the Raspberry Pi via USB. The alko deamon is serving a simple admin interface is over HTTP.

The Arduino is replacing the original Dixie Narco controller board in the vending machine and is controlling the LCD, as well as the vending machine motors and sensors.

## License
* The project source code is licensed under Apache 2.0. 
* The included libraries (QtService, MCP23017 and LiquidCrystal) are licensed under a different license - please check the source code.

## Admin web interface

![Admin web interface status page](/images/alko-web-status.png)
![Admin web interface control panel page](/images/alko-web-admin.png)
