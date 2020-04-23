# Yamaha YM2149 ESP32 Soundmodule

##_ATTENTION_
**_DON'T FLASH THE ESP32 WHILE CONNECTED TO POWERSUPPLY ! THIS COULD DESTROY THE ESP32 ! JUST USE AN USB CABLE !_**

## Abstract
This soundmodule enables YM2149 soundgeneration via a web interface in a dedicated WiFi Network or your existing WLAN. 

The YM2149 is Yamaha's pin- and register compatible version of [General Instrument's AY-3-8910](https://en.wikipedia.org/wiki/General_Instrument_AY-3-8910) which was used in legendary video game consoles and home computers like
- Intellivision
- Vectrex
- Amstrad CPC
- MSX
- later ZX Spectrum

The soundchip is controlled by an [Espressif ESP32](ttps://www.espressif.com/en/products/socs/esp32/overview) which also works as
- Webserver providing HTML/JavaScript documents to control the soundchip via browser
- Application Server providing a REST interface
- clock generator for YM2149
- (WiFi Access Point)

## HW Revision 1.0

### Features
- HW-104 (PAM8403) Amplifier Board
- Breadboard Powersupply(3V3 and 5V) Board 
- ESP32-WROOM Devkit Board
- ALPS RV16 A10K logarithmic potentiometer used
- 6.3mm Audiojack

### Bugs
[ ] Mounting holes at random positions
