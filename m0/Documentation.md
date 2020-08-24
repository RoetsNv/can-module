# Implementation of CAN protocol in Ginco

This document defines the way CAN capable devices will communicate on the bus with the Ginco Home Automation server running on the Raspberry pi using the CAN protocol.

## Custom communication protocol

Many communicaton protocols are developed for CAN, for example CANopen. But for simplicity we designed our own protocol since CANopen does not offer interesting features for a home automation system where precise timen and arbitration of the bus isn't nessecary.


## CAN modules
These are primarily microcontrollers that will send data over the CAN bus. One module can be responsible for multiple tasks. For example one module can contain a temperature sensor, a push button, a status LED,...

CAN does not support Source destination adresses. We use the message ID field of a CAN message for to define some sort of source adress.

**Each CAN module has a static message id (1 byte in hex)** It will always use this adress to  

These adresses can be adjusted for each module depending on their importance due to CAN bus arbritration. The lower the adress the sooner the message will be received by other modules on the bus.
Since one module serves multiple purposes, and we need to determine what we're talking about we need some kind of identifier. To solve this **each CAN module has multiple 'ports'** We define the port in the **first byte** of each message.

## Message formats
The first 2 bytes of a message will never contain data, first byte defines the port (as mentioned above). The **Second byte describes the type of IO** for example button, rotary button, RGBLED,...

Further data fields are filled with event data e.g. buttonpress, temperature,...

### Types of IO
* Button:
  1. Released (0x00) or Pressed (0x01)
  1. Duration byte 1
  1. Duration byte 2
* Sensor:
 