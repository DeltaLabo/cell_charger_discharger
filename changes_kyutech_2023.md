Changes made in Kyutech from December 11-15
============

## Objectives
* Eliminate all the ASCII text from memory using a configuration frame instead of a menu
* Program a correct PID implementation usign the developed algoritm and floats instead of longs
* Test Ni-MH with new algoritm and stablish the correct constants

## Structure of commands
|start byte | operation | command code | length | data | checksum | stop byte |
|--|--|--|--|--|--|--|
0xDD | 1byte | 1byte | 1byte | 20bytes max | 2bytes | 0x77

### operation:
* 0xA5: read
* 0x5A: write 

### command code:
* 0x03: read basic configuration 
* 0x05: write basic configuration
* 0x07: read test configuration
* 0x09: write test configuration
* 0x0B: read converter configuration
* 0x0D: write converter configuration
* 0x0F: action commands

### length:
* min: 0x00 (0)
* max: 0x14 (20)

### checksum
* byte by byte sum of _command code_, _lenght_ and _data_

## Basic configuration

| version | const. voltage | const. current | capacity | end-of-charge | end-of-precharge | end-of-discharge | end-of-postdischarge |
|--|--|--|--|--|--|--|--|
| 1byte | 2bytes | 2bytes | 2bytes | 2 bytes | 2 bytes | 2bytes | 2bytes |

**Note:** in all cases MSBF

### version
* 0x01: Li-Ion
* 0x02: Ni-MH

### const. voltage
* value of constant voltage (mV)

### const. current
* value of constant current (mA)

### capacity
* value of capacity (mAh)

### end-of-charge
* Li-Ion: value of end of charge current (mA)
* Ni-MH: value of end of charge delta V (mV)

### end-of-precharge 
* Li-Ion: value of end of precharge current (mA)
* Ni-MH: value of end of precharge delta V (mV)

### end-of-discharge
* value of end of discharge voltage (mV)

### end-of-postdischarge
* value of end of postdischarge capacity (mAh)

### Example to write and read basic configuration
The user sends the _write basic configuration_ command:

DD 5A 05 0F 01 10 68 0D AC 0D AC 00 64 00 64 09 C4 06 D6 3D 37 77

start byte (0xDD), operation: write (0x5A), command: write basic configuration (0x05), length: 15 (0x0F), version: Li-Ion (0x01), CV value: 4200mV (0x1068), CC value: 3500mA (0x0DAC), capacity: 3500mAh (0x0DAC), end-of-charge current: 100mA (0x0064), end-of-precharge current: 100mA (0x0064), end-of-discharge voltage: 2500mV (0x09C4), end-of-postdischarge capacity: 1750mAh (0x06D6), checksum: 15671 (0x3D37) and stop byte (0x77)

Nothing should be received.

Then, the user sends the _read basic configuration_ command: 

DD A5 03 00 00 03 77

start byte (0xDD), operation: read (0xA5), command: read basic configuration (0x03), size of data: 0 (0x00), checksum: 3 (0x0003) and stop byte (0x77) 

the user receives:

DD A5 03 0B 01 10 68 0D AC 0D AC 00 64 00 64 09 C4 06 D6 3D 30 77

operation byte: operation: read (0xA5), command: read basic configuration (0x03), checksum: 15669 (0x3D35), the start, stop and data bytes should be the same

## Test configuration

| number of cells | number of states | number of repetitions | order of states | wait time | end wait time | 
|--|--|--|--|--|--|
| 1byte | 1byte | 1byte | 12bytes max | 2bytes | 2bytes |

### states definition

* charge = 0x03
* precharge = 0x05
* discharge = 0x07
* postdischarge = 0x09
* DC resistance = 0x0B

### Example to write and read test configuration
The user sends the _write test configuration_ command:

DD 5A 09 11 01 08 01 05 0B 07 0B 03 0B 09 0B 02 58 04 B0 07 6D 77

start byte (0xDD), operation: write (0x5A), command: write test configuration (0x09), length: 17 (0x11), number of cells: 1 (0x01), number of states: 8 (0x08), number of repetitions: 1 (0x01), order of states: precharge -> DC res -> discharge -> DC res -> charge -> DC res -> postdischarge -> DC res (0x05 0x0B 0x07 0x0B 0x03 0x0B 0x09 0x0B), wait time between states: 600s (0x0258), end wait time: 1200s (04B0), checksum: 1901 (0x076D), stop byte (0x77)

Nothing should be received

Then, the user sends the _read test configuration_ command: 

DD A5 07 00 00 09 77

start byte (0xDD), operation: read (0xA5), command: read test configuration (0x07), size of data: 0 (0x00), checksum: 9 (0x0009) and stop byte (0x77) 

the user receives:

DD A5 07 11 01 08 01 05 0B 07 0B 03 0B 09 0B 02 58 04 B0 07 6B 77

operation: read (0xA5), command: read test configuration (0x07), checksum: 1899 (0x076B) and the start, stop and data bytes should be the same

## Converter configuration

| CVKp | CVKi | CVKd | CCKp | CCKi |
|--|--|--|--|--|
| 2bytes | 2bytes | 2bytes | 2bytes | 2bytes |

**Note:** in all cases MSBF

### CVKp
* value of proportional coefficient for constant voltage (x1E10-3)

### CVKi
* value of integral coefficient for constant voltage (x1E10-3)

### CVKd
* value of differential coefficient for constant voltage (x1E10-1)

### CCKp
* value of proportional coefficient for constant current (x1E10-3)

### CCKi
* value of integral coefficient for constant current (x1E10-3)

### Example to write and read converter configuration
The user sends the _write converter configuration_ command:

DD 5A 0D 0A 0B F6 00 03 05 0B 06 B0 00 9B 18 64 77

start byte (0xDD), operation: write (0x5A), command: write converter configuration (0x0D), length: 10 (0x0A), CVKp: 3.062 (0x0BF6), CVKi: 0.003 (0x0003), CVKd: 129.1 (0x050B), CCkp: 1.712 (0x06B0), CCKi: 0.155 (0x009B), checksum: 6246 (0x1866) and stop byte (0x77)

Nothing should be received

Then, the user sends the _read converter configuration_ command: 

DD A5 0B 00 00 0B 77

start byte (0xDD), operation: read (0xA5), command: read converter configuration (0x0B), length: 0 (0x00), checksum: 11 (0x000B) and stop byte (0x77) 

the user receives:

DD A5 0B 0A 0B F6 00 03 05 0B 06 B0 00 9B 18 64 77

operation: read (0xA5), command: read coverter configuration (0x0B), checksum: 6244 (0x1864) and the start, stop and data bytes should be the same

## Action commands

| Action command code | Parameters |
|--|--|
| 2bytes | 2bytes |

### Action command code
* 0x03: reset
* 0x05: start
* 0x07: next cell
* 0x09: next state

### Example to write action command


## Structure of log data

| cell counter | repetition counter | state | elapsed time | voltage | current | capacity | temperature | duty cycle |
|--|--|--|--|--|--|--|--|--|
| 1bytes | 1bytes |  
