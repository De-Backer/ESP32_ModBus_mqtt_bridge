# An ESP32 Modbus Mqtt bridge to node red.

## Purpose
To the works like 'modbus-flex-getter' and 'modbus-flex-write' of https://flows.nodered.org/node/node-red-contrib-modbus.
Which then enables us to read multiple devices over the internet (or local network), without multiple PC's or raspberry pi's.
eg:
```
device <-> modbus <-> ESP32 <-> Mqtt-Broker <-> a 'node red' able device (PC, Rpi, ...)
device <-> modbus <-> ESP32 <│
device <-> modbus <-> ESP32 <|
```
### node red 'modbus-flex-getter' and 'modbus-flex-write'
We use mqtt for the modbus-flex-getter and modbus-flex-write.

![Node-red modbus-flex-getter and modbus-flex-write Screenshot](/Node-red/node-red-contrib-modbus.png)
![Node-red modbus-flex-getter and modbus-flex-write Screenshot](/Node-red/Screenshot.png)
[Node-red code](/Node-red/inport.txt)

## Status of project
In the test phase.
## works:
 - [ ] FC 1: Read Coil Status
 - [ ] FC 2: Read Input Status
 - [X] FC 3: Read Holding Registers
 - [x] FC 4: Read Input Registers
 - [ ] FC 5: Force Single Coil
 - [x] FC 6: Preset Single Register
 - [ ] FC 15: Force Multiple Coils
 - [X] FC 16: Preset Multiple Registers

## To build
At Visual Studio Code, SDK Configuration editor of ESP-IDF SDK mod:

App Configuration
- Broker URL
- slave ip address
- slave read request mqtt
- slave write request mqtt
- slave read response mqtt
- slave write response mqtt
- status mqtt

Example Connection Configuration
- WiFi SSID
- WiFi Password
- WiFi Scan auth mode threshold

Modbus configuration
- Modbus RTU/ASCII/TCP

Modbus RTU/ASCII
- UART port number
- UART RXD pin number
- UART TXD pin number
- UART RTS pin number
- UART communication speed

Modbus TCP
- Modbus TCP port number
