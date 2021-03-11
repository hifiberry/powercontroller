# Communicating with the power controller

## Flash firmware

Flashing the firmware is handled by Pi's serial port. Have a look at [this document](flashing.md).

## Setting parameters

Communication with the power controller during normal operating is handled via I2C. The pwoer controller uses a set 
of 8-bit registers to set or get the value of specific parameters. For a full list of registers, have a look at the file 
[registers.h](powercontrol/registers.h).

Default I2C address is 0x77.

### Examples

#### Get LED mode
```
i2cget -y -f 1 0x77 1
```

### Set LED to flashing blue
```
i2cset -y -f 1 0x77 1 3  # mode: blinking
i2cset -y -f 1 0x77 2 0  # no red
i2cset -y -f 1 0x77 3 0  # no green
i2cset -y -f 1 0x77 4 0xff  # but blue
```

