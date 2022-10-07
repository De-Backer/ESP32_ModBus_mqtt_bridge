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
### node red 'modbus-flex-getter' and 'modbus-flex-write'
We use mqtt for the modbus-flex-getter and modbus-flex-write.

![Node-red modbus-flex-getter and modbus-flex-write Screenshot](/Node-red/node-red-contrib-modbus.png)
![Node-red modbus-flex-getter and modbus-flex-write Screenshot](/Node-red/Screenshot.png)
```
[
    {
        "id": "3741aeadec5c7c10",
        "type": "mqtt in",
        "z": "8bb4dd777cbd9ec1",
        "name": "",
        "topic": "response/read",
        "qos": "2",
        "datatype": "json",
        "broker": "35117e19fbb59713",
        "nl": false,
        "rap": true,
        "rh": 0,
        "inputs": 0,
        "x": 570,
        "y": 720,
        "wires": [
            [
                "bc37b89ed5b96df8"
            ]
        ]
    },
    {
        "id": "bc37b89ed5b96df8",
        "type": "function",
        "z": "8bb4dd777cbd9ec1",
        "name": "from payload",
        "func": "var rawData = new ArrayBuffer(4);\nvar int8View = new Uint8Array(rawData);\nvar intView = new Uint16Array(rawData);\nvar fltView = new Float32Array(rawData);\n\nvar buffer = [];\nvar floatdata = [];\nfor(let at_p=0;at_p< (msg.payload.data.length-2);at_p+=2){\n\tintView[0] = msg.payload.data[at_p+1];\n\tintView[1] = msg.payload.data[at_p];\n\tfloatdata.push(fltView[0]);\n\tbuffer.push(int8View[0]);\n\tbuffer.push(int8View[1]);\n\tbuffer.push(int8View[2]);\n\tbuffer.push(int8View[3]);\n}\n\nvar newMsg = { \n    topic:msg.payload.topic,\n    payload: {\n        data:msg.payload.data,\n        buffer:buffer,\n        floatdata:floatdata\n    },\n    modbusRequest:msg.payload.modbusRequest,\n    values:msg.payload.data};\nreturn newMsg;",
        "outputs": 1,
        "noerr": 0,
        "initialize": "",
        "finalize": "",
        "libs": [],
        "x": 730,
        "y": 720,
        "wires": [
            []
        ]
    },
    {
        "id": "ac77e577eb0e07d8",
        "type": "mqtt out",
        "z": "8bb4dd777cbd9ec1",
        "name": "",
        "topic": "request/read",
        "qos": "0",
        "retain": "",
        "respTopic": "",
        "contentType": "application/json",
        "userProps": "",
        "correl": "",
        "expiry": "10",
        "broker": "35117e19fbb59713",
        "x": 570,
        "y": 680,
        "wires": []
    },
    {
        "id": "f3b249c2f7a4653b",
        "type": "function",
        "z": "8bb4dd777cbd9ec1",
        "name": "add to payload",
        "func": "var topic = \"\";\ntopic = msg.topic;\nmsg.payload.topic = topic;\nreturn msg;",
        "outputs": 1,
        "noerr": 0,
        "initialize": "",
        "finalize": "",
        "libs": [],
        "x": 400,
        "y": 680,
        "wires": [
            [
                "ac77e577eb0e07d8"
            ]
        ]
    },
    {
        "id": "01f8abdbac6d8253",
        "type": "mqtt in",
        "z": "8bb4dd777cbd9ec1",
        "name": "",
        "topic": "response/write",
        "qos": "2",
        "datatype": "json",
        "broker": "35117e19fbb59713",
        "nl": false,
        "rap": true,
        "rh": 0,
        "inputs": 0,
        "x": 580,
        "y": 840,
        "wires": [
            [
                "77ee97239959cb1a"
            ]
        ]
    },
    {
        "id": "77ee97239959cb1a",
        "type": "function",
        "z": "8bb4dd777cbd9ec1",
        "name": "from payload",
        "func": "var rawData = new ArrayBuffer(4);\nvar int8View = new Uint8Array(rawData);\nvar intView = new Uint16Array(rawData);\nvar fltView = new Float32Array(rawData);\n\nvar buffer = [];\nvar floatdata = [];\nfor(let at_p=0;at_p< (msg.payload.data.length-2);at_p+=2){\n\tintView[0] = msg.payload.data[at_p+1];\n\tintView[1] = msg.payload.data[at_p];\n\tfloatdata.push(fltView[0]);\n\tbuffer.push(int8View[0]);\n\tbuffer.push(int8View[1]);\n\tbuffer.push(int8View[2]);\n\tbuffer.push(int8View[3]);\n}\n\nvar newMsg = { \n    topic:msg.payload.topic,\n    payload: {\n        data:msg.payload.data,\n        buffer:buffer,\n        floatdata:floatdata\n    },\n    modbusRequest:msg.payload.modbusRequest,\n    values:msg.payload.data};\nreturn newMsg;",
        "outputs": 1,
        "noerr": 0,
        "initialize": "",
        "finalize": "",
        "libs": [],
        "x": 750,
        "y": 840,
        "wires": [
            []
        ]
    },
    {
        "id": "ab7d57fbea364de9",
        "type": "mqtt out",
        "z": "8bb4dd777cbd9ec1",
        "name": "",
        "topic": "request/write",
        "qos": "0",
        "retain": "",
        "respTopic": "",
        "contentType": "application/json",
        "userProps": "",
        "correl": "",
        "expiry": "10",
        "broker": "35117e19fbb59713",
        "x": 570,
        "y": 800,
        "wires": []
    },
    {
        "id": "980eed82e9d8f4be",
        "type": "function",
        "z": "8bb4dd777cbd9ec1",
        "name": "add to payload",
        "func": "var topic =\"\";\ntopic = msg.topic;\nmsg.payload.topic=topic;\nreturn msg;",
        "outputs": 1,
        "noerr": 0,
        "initialize": "",
        "finalize": "",
        "libs": [],
        "x": 400,
        "y": 800,
        "wires": [
            [
                "ab7d57fbea364de9"
            ]
        ]
    },
    {
        "id": "1fe92864252a702b",
        "type": "mqtt in",
        "z": "8bb4dd777cbd9ec1",
        "name": "",
        "topic": "status",
        "qos": "2",
        "datatype": "auto",
        "broker": "35117e19fbb59713",
        "nl": false,
        "rap": true,
        "rh": 0,
        "inputs": 0,
        "x": 570,
        "y": 920,
        "wires": [
            [
                "dce4f1f5bc93c718"
            ]
        ]
    },
    {
        "id": "dce4f1f5bc93c718",
        "type": "ui_text",
        "z": "8bb4dd777cbd9ec1",
        "group": "af9e406f226671cf",
        "order": 1,
        "width": 0,
        "height": 0,
        "name": "",
        "label": "{{msg.topic}}",
        "format": "{{msg.payload}}",
        "layout": "row-spread",
        "className": "",
        "x": 710,
        "y": 920,
        "wires": []
    },
    {
        "id": "4845fcb2d7ac5b2a",
        "type": "comment",
        "z": "8bb4dd777cbd9ec1",
        "name": "modbus read",
        "info": "",
        "x": 390,
        "y": 640,
        "wires": []
    },
    {
        "id": "448e1b590b63d44e",
        "type": "comment",
        "z": "8bb4dd777cbd9ec1",
        "name": "modbus write",
        "info": "",
        "x": 390,
        "y": 760,
        "wires": []
    },
    {
        "id": "35117e19fbb59713",
        "type": "mqtt-broker",
        "name": "",
        "broker": "127.0.0.1",
        "port": "1883",
        "clientid": "",
        "autoConnect": true,
        "usetls": false,
        "protocolVersion": "4",
        "keepalive": "60",
        "cleansession": true,
        "birthTopic": "",
        "birthQos": "0",
        "birthPayload": "",
        "birthMsg": {},
        "closeTopic": "",
        "closeQos": "0",
        "closePayload": "",
        "closeMsg": {},
        "willTopic": "",
        "willQos": "0",
        "willPayload": "",
        "willMsg": {},
        "userProps": "",
        "sessionExpiry": "",
        "credentials": {
            "user": "",
            "password": ""
        }
    },
    {
        "id": "af9e406f226671cf",
        "type": "ui_group",
        "name": "Default",
        "tab": "153ff981bc6e38e0",
        "order": 1,
        "disp": true,
        "width": "27",
        "collapse": false,
        "className": ""
    },
    {
        "id": "153ff981bc6e38e0",
        "type": "ui_tab",
        "name": "Home",
        "icon": "dashboard",
        "disabled": false,
        "hidden": false
    }
]
```
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
