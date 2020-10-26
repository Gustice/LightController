# Light Controller

## Known-Isses and open Tasks
- Power tree should provide some option to measue actual input consumption
- SD-Card is not implemented yet

## Scope

### Lights

- Synchronous LED-strips (based on APA102)
- Asynchronous LED-strips (based on WS2812)
- RGB-LED-Stripes with Load switches
- RGB-LED in pulsed current control mode
- LED in pulsed current control mode (with higher power dissipation)

### Electrical Specification
- Note not to exceed power consumption fo 5V/500mA on USB-Port or 5V/1A on Socket
- The Boost converter for internal 12V-potential is designed for max 200 mA 

### SD-Card
The SD will provide storage for web-based control interface

### Controls

#### Encoder
The Encoder is intendet to control brighness and/or different LightProgramms
#### Potentiometer
The potentiometers are intendet to control brightnes of RGB-Channels and White-Channel separately
#### Swtichtes
The Switches are intendet to switch different LightPrograms or to switch/reset WiFi-Configuration
#### IR-Port
The IR-Port will provide a control interface for Phillips compatible remote controls

### Extensions

- I2C LED Driver

## Usage

### By Hosted Webpage

### By Smartphone App

### By Remote Control
