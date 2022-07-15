# An ESP32 TCP Modbus Mqtt bridge to node red.

## Purpose
To the works like 'modbus-flex-getter' and 'modbus-flex-write' of https://flows.nodered.org/node/node-red-contrib-modbus.
Which then enables us to read multiple devices over the internet (or local network), without multiple PC's or raspberry pi's.
eg:
```
device <-> modbus <-> ESP32 <-> internet <-> a 'node red' able device (PC, Rpi, ...)
device <-> modbus <-> ESP32 <│
device <-> modbus <-> ESP32 <|
```
## Status of project
In the test phase.
