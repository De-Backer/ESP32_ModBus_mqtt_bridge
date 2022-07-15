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
[{"id":"b4666883.926208","type":"mqtt in","z":"2592ac53.d7c284","name":"","topic":"/response/read","qos":"2","datatype":"json","broker":"bf0faca0.08cc78","nl":false,"rap":true,"rh":0,"x":690,"y":260,"wires":[["79c2e9cb.d394e8"]]},{"id":"b25e2c54.d0add","type":"mqtt in","z":"2592ac53.d7c284","name":"","topic":"/response/write","qos":"2","datatype":"json","broker":"bf0faca0.08cc78","nl":false,"rap":true,"rh":0,"x":690,"y":500,"wires":[["93ffb2e4.4be06"]]},{"id":"6f680cd7.311d54","type":"mqtt out","z":"2592ac53.d7c284","name":"","topic":"/request/read","qos":"0","retain":"","respTopic":"","contentType":"","userProps":"","correl":"","expiry":"","broker":"bf0faca0.08cc78","x":690,"y":220,"wires":[]},{"id":"d72a9cf0.b354c","type":"mqtt out","z":"2592ac53.d7c284","name":"","topic":"/request/write","qos":"0","retain":"","respTopic":"","contentType":"","userProps":"","correl":"","expiry":"","broker":"bf0faca0.08cc78","x":690,"y":460,"wires":[]},{"id":"fdab0f03.bd0d9","type":"comment","z":"2592ac53.d7c284","name":"modbus read","info":"","x":610,"y":140,"wires":[]},{"id":"839c0e1e.49fee","type":"comment","z":"2592ac53.d7c284","name":"modbus write","info":"","x":610,"y":380,"wires":[]},{"id":"8432ab1c.a0a1f8","type":"function","z":"2592ac53.d7c284","name":"add to payload","func":"msg.payload.topic = msg.topic;\nreturn msg;","outputs":1,"noerr":0,"initialize":"","finalize":"","libs":[],"x":620,"y":420,"wires":[["d72a9cf0.b354c","598aeade.e19e74"]]},{"id":"93ffb2e4.4be06","type":"function","z":"2592ac53.d7c284","name":"from payload","func":"var newMsg = { \n    topic:msg.payload.topic,\n    payload: {\n        data:msg.payload.data,\n        buffer:msg.payload.buffer\n    },\n    modbusRequest:msg.payload.modbusRequest,\n    values:msg.payload.values};\n    \n\nreturn newMsg;","outputs":1,"noerr":0,"initialize":"","finalize":"","libs":[],"x":770,"y":540,"wires":[[]]},{"id":"530f52e5.77b96c","type":"function","z":"2592ac53.d7c284","name":"add to payload","func":"msg.payload.topic = msg.topic;\nreturn msg;","outputs":1,"noerr":0,"initialize":"","finalize":"","libs":[],"x":620,"y":180,"wires":[["6f680cd7.311d54","70052935.3da088"]]},{"id":"79c2e9cb.d394e8","type":"function","z":"2592ac53.d7c284","name":"from payload","func":"var newMsg = { \n    topic:msg.payload.topic,\n    payload: {\n        data:msg.payload.data,\n        buffer:msg.payload.buffer\n    },\n    modbusRequest:msg.payload.modbusRequest,\n    values:msg.payload.values};\n    \n\nreturn newMsg;","outputs":1,"noerr":0,"initialize":"","finalize":"","libs":[],"x":770,"y":300,"wires":[[]]},{"id":"70052935.3da088","type":"debug","z":"2592ac53.d7c284","name":"","active":false,"tosidebar":true,"console":false,"tostatus":false,"complete":"true","targetType":"full","statusVal":"","statusType":"auto","x":790,"y":180,"wires":[]},{"id":"598aeade.e19e74","type":"debug","z":"2592ac53.d7c284","name":"","active":false,"tosidebar":true,"console":false,"tostatus":false,"complete":"true","targetType":"full","statusVal":"","statusType":"auto","x":790,"y":420,"wires":[]},{"id":"bf0faca0.08cc78","type":"mqtt-broker","name":"","broker":"a_broker.com","port":"1883","clientid":"","usetls":false,"protocolVersion":"4","keepalive":"60","cleansession":true,"birthTopic":"","birthQos":"0","birthRetain":"true","birthPayload":"","birthMsg":{},"closeTopic":"","closePayload":"","closeMsg":{},"willTopic":"","willQos":"0","willPayload":"","willMsg":{},"sessionExpiry":""}]
```
## Status of project
In the test phase.

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
- Modbus TCP port number
