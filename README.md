# LightController

A ESP32 based LED-Controller for various light scenarios.

## Purpose

This project is intended to be used as base platform for various Light projects. Its based on the ESP32 WiFi SoC and utilizes the Espressif development framework (ESP-IDF). 

On top of that this is a small hardware and board abstraction layer that simplifies the hardware access and provides further bases for device implementation.

The main purpose is of course to have fun an learn several new techniques and hacks. I also hoped to dig into a promising WiFi platform that enables a wide band of applications. As I happily discovered it was absolutely worth the time and I'm glad that I didn't stuck to arduino.

The intended Products that should result from this are extreme fancy night lights for bedrooms of my children and the stairwell and low key disco lights / ambient lights for the living room.

## Platform

This platform is capable of driving most LEDs I encountered so far (at least the very most common LEDs I have seen so far). 

![Main Board](./Documentation/Pics/Board.png)

- Ordinary RGB-Stripes (optionally with white channel)
- Serial LED-Stripes of both both types
  - Most common WS2812 (with RTZ-protocol)
  - Less common APA102 (with easy to use SPI)
- I2C-Expander for single or chains of LEDs

The main board integrates two switches, two analogue inputs and 1 encoder to be integrated in controlling of the station. It also can utilize a UART interface to another protocol bridge or a infrared interface which are also both prepared (the IR-Interface would come at cost of one digital function).

The board also aims to drive short LED-strips solidly with 5 V power supply. Hence it integrates also a small boost-converter.

The next picture shows a block diagram of the platform. Note that the expander board is not designed yet and the main-board will be eventually redesigned to be smaller an perhaps also provide a battery management circuitry.

![Block Diagramm of Light platform](./Documentation/Pics/BlockDiagramm.png)

The next picture shows the first Device I build up. It is solidly for implementation purpose.

![Function Demonstrator](./Documentation/Pics/FunctionDemo.png)

The next picture shows a web GUI page to control the LED colors. It currently only supports to control exactly the first LED of each channel.

![Web GUI](./Documentation/Pics/WebGui.png)

## Usage

There is currently not very much useful stuff going on. But to get faster familiar with this project this could help you to read. 

After powerup the device provides a WiFi-access point. 

- SSID `cLight`
- Password `FiatLuxx`

You can connect to the device and display the root page by typing "192.168.4.1/" (in my case). On this page you see some (currently) little relevant in formation and can navigate to set up your WiFi-parameter or to setup the Lights.

- If you setup the WiFi-parameter the next time the device will try to connect to the provided station. You can Reset the parameter by pressing button 1 at start up 
- If you setup the LEDs you currently can only setup the first LED-slave.

## Development

The development is done in *Visual Studio Code* so far. To be able to join the development you must have the [IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html) installed. Add also the path to the installed *IDF* as system path variable.

To be able to compile the code you must start vs code from command line with exported environment variables

- Open console in your project path
- Type `%IDF_PATH%\export.bat` to setup the environment for used toolchain
- Type `code .` to open current path in *VS Code* 
- No you are able to start build-process by just hitting `Ctrl+Shift+B`

To be able to flash your ESP you need a USB to UART bridge, that also exposes RTS and DTR pins. These are needed to reset and set the ESP32 into programming mode.

## Issues

This project is anything but finished. I have several open tasks and they somehow appear to increase rather than getting less. 

- Status LEDs don't display any status yet.
- Light-Control state machine is still not integrated. Currently the is no soft switching or any prepared effect used. 
- The memory-pages don't work currently. They are intended to save current color configuration to be called by remote or similar devices.
- The is some kind of configuration management completely missing. It would be necessary to provide a flash stored configuration of the device which introduces used interfaces and LED count so the GUI could be rendered accordingly
- The web GUI is as simple as it can be. There are no web sockets implemented or anything else. 
- Linear power states with configurable current flow are not designed yet.

## ToDos

- Integration of Light-Controller with soft-switching and light effects
- Layout and description of sockets and buttons
- Design for linear Endstage (expander board)
- Adaption of ZigBee
- Interactive GUI with true Rest-Interface

