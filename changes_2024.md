Changes made in Costa Rica 2024
============

## Objectives
* Change the frames for a proper configuration of CV and CC PID constants.
* Change the frames for a proper interruption handling while running.

## Structure of commands
|start byte | operation | command code | length | data | checksum | stop byte |
|--|--|--|--|--|--|--|
0xDD | 1byte | 1byte | 1byte | 20bytes max | 2bytes | 0x77

### operation:
* 0xA5: read
* 0x5A: write 
* 0x0F: action

### read/write command codes:
* 0x03: basic configuration 
* 0x05: test configuration
* 0x07: converter configuration

### action command codes
* 0x03: reset
* 0x05: start
* 0x07: next cell
* 0x09: next state

### length:
* min: 0x00 (0)
* max: 0x14 (20)

### checksum
* byte by byte sum of _command code_, _lenght_ and _data_

## Converter configuration

| CVKp | CVKi | CVKd | CCKp_Charge | CCKi_Charge | CCKp_Discharge | CCKi_Discharge |
|--|--|--|--|--|--|--|
| 2bytes | 2bytes | 2bytes | 2bytes | 2bytes | 2bytes | 2bytes |

**Note:** in all cases MSBF

### CVKp
* value of proportional coefficient for constant voltage (x1E10-6)

### CVKi
* value of integral coefficient for constant voltage (x1E10-6)

### CVKd
* value of differential coefficient for constant voltage (x1E10-3)

### CCKp Charge
* value of proportional coefficient for constant voltage (x1E10-6)

### CCKi Charge
* value of integral coefficient for constant voltage (x1E10-6)

### CCKp Discharge
* value of proportional coefficient for constant current (x1E10-6)

### CCKi Discharge
* value of integral coefficient for constant current (x1E10-6)

## Actions
Action commands must be one byte long excepting the start. The ones that are one byte long, can only be interpreted while the system is active.

### Example of action commands
to reset:

03

to start:

DD 0F 05 00 00 05 77

go to next cell:

07

go to next state:

09