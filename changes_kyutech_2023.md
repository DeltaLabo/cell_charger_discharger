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

## Basic configuration

| version | const. voltage | end-of-postdischarge | end-of-discharge | end-of-precharge | end-of-charge | capacity | const-current-charge | const-current-discharge |
|--|--|--|--|--|--|--|--|--|
| 1byte | 2bytes | 2bytes | 2bytes | 2 bytes | 2 bytes | 2bytes | 2bytes | 2bytes |

**Note:** in all cases MSBF

### version
* 0x01: Li-Ion
* 0x02: Ni-MH

### const. voltage
* value of constant voltage (mV)
### end-of-postdischarge
* value of end of postdischarge capacity (mAh)

### end-of-discharge
* value of end of discharge voltage (mV)

### end-of-precharge 
* Li-Ion: value of end of precharge current (mA)
* Ni-MH: value of end of precharge delta V (mV)

### end-of-charge
* Li-Ion: value of end of charge current (mA)
* Ni-MH: value of end of charge delta V (mV)
  
### const-current-charge
* value of constant current when charging (mA)
  
### const-current-discharge
* value of constant current when discharging (mA)

### capacity
* value of capacity (mAh)







### Example to write and read basic configuration
The user sends the _write basic configuration_ command:

DD 5A 03 0F 01 10 68 0D AC 0D AC 00 64 00 64 09 C4 06 D6 04 6E 77

start byte (0xDD), operation: write (0x5A), command: basic configuration (0x03), length: 15 (0x0F), version: Li-Ion (0x01), CV value: 4200mV (0x1068), CC value: 3500mA (0x0DAC), capacity: 3500mAh (0x0DAC), end-of-charge current: 100mA (0x0064), end-of-precharge current: 100mA (0x0064), end-of-discharge voltage: 2500mV (0x09C4), end-of-postdischarge capacity: 1750mAh (0x06D6), checksum: 1134 (0x046E) and stop byte (0x77)

Nothing should be received.

Then, the user sends the _read basic configuration_ command: 

DD A5 03 00 00 03 77

start byte (0xDD), operation: read (0xA5), command: basic configuration (0x03), size of data: 0 (0x00), checksum: 3 (0x0003) and stop byte (0x77) 

the user receives:

DD A5 03 0F 01 10 68 0D AC 0D AC 00 64 00 64 09 C4 06 D6 04 6E 77

operation byte: operation: read (0xA5), command: basic configuration (0x03), checksum: 1134 (0x046E), the start, stop and data bytes should be the same

## Test configuration

| number of cells | number of states | number of repetitions | wait time | end wait time | order of states | 
|--|--|--|--|--|--|
| 1byte | 1byte | 1byte | 2bytes | 2bytes | 10bytes max |

### states definition

* charge = 0x03
* precharge = 0x05
* discharge = 0x07
* postdischarge = 0x09
* DC resistance = 0x0B
* undefinded = 0x00

### Example to write and read test configuration
The user sends the _write test configuration_ command:

DD 5A 05 11 01 08 01 02 58 04 B0 05 0B 07 0B 03 0B 09 0B 00 00 01 6B 77

start byte (0xDD), operation: write (0x5A), command: test configuration (0x05), length: 17 (0x11), number of cells: 1 (0x01), number of states: 8 (0x08), number of repetitions: 1 (0x01), wait time between states: 600s (0x0258), end wait time: 1200s (04B0), order of states: precharge -> DC res -> discharge -> DC res -> charge -> DC res -> postdischarge -> DC res and two undefined states (0x05 0x0B 0x07 0x0B 0x03 0x0B 0x09 0x0B 0x00 0x00), checksum: 363 (0x016B), stop byte (0x77)

Nothing should be received

Then, the user sends the _read test configuration_ command: 

DD A5 05 00 00 09 77

start byte (0xDD), operation: read (0xA5), command: test configuration (0x05), size of data: 0 (0x00), checksum: 5 (0x0009) and stop byte (0x77) 

the user receives:

DD A5 05 11 01 08 01 05 0B 07 0B 03 0B 09 0B 02 58 04 B0 00 00 01 6B 77

operation: read (0xA5), command: test configuration (0x05), checksum: 363 (0x016B) and the start, stop and data bytes should be the same

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

DD 5A 07 0A 0B F6 00 03 05 0B 06 B0 00 9B 18 64 77

start byte (0xDD), operation: write (0x5A), command: converter configuration (0x07), length: 10 (0x0A), CVKp: 3.062 (0x0BF6), CVKi: 0.003 (0x0003), CVKd: 129.1 (0x050B), CCkp: 1.712 (0x06B0), CCKi: 0.155 (0x009B), checksum: 6242 (0x1862) and stop byte (0x77)

Nothing should be received

Then, the user sends the _read converter configuration_ command: 

DD A5 0B 00 00 0B 77

start byte (0xDD), operation: read (0xA5), command: converter configuration (0x07), length: 0 (0x00), checksum: 7 (0x0007) and stop byte (0x77) 

the user receives:

DD A5 0B 0A 0B F6 00 03 05 0B 06 B0 00 9B 18 64 77

operation: read (0xA5), command: read coverter configuration (0x0B), checksum: 6242 (0x1862) and the start, stop and data bytes should be the same

## Actions
Action commands do not have any data 

### Example of action commands
to reset:

DD 0F 03 00 03 77

to start:

DD 0F 05 00 05 77

go to next cell:

DD 0F 07 00 07 77

go to next state:

DD 0F 09 00 09 77

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

###

## Structure of log data

| start byte | cell counter | repetition counter | state | elapsed time | voltage | current | capacity | temperature | duty cycle | stop byte |
|--|--|--|--|--|--|--|--|--|--|--|
| 0xDD | 1byte | 1byte | 1byte | 2bytes | 2bytes | 2bytes | 2bytes | 2bytes | 2bytes | 0x77 |

### Example of log data

After the user finish read the configuration commands and the start command is sent the system will start sendind data every second

The user will receive

DD 01 01 03 00 64 0F 0A 06 D6 0C B2 09 68 02 A6 77

cell counter: 1 (0x01), repetition counter: 1 (0x01), state: charge (0x03), elapsed time: 100s (0x0064), voltage: 3850mV (0x0F0A), current: 1725mA (0x06D6), capacity: 3250mAh (0x0CB2), temperature: 24.08C (0x0968), duty cycle: 0.678 (0x02A6)

