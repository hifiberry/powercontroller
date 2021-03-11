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

#### Set LED to flashing blue
```
i2cset -y -f 1 0x77 0x01 3  # mode: flashing
i2cset -y -f 1 0x77 0x02 0x00  # R
i2cset -y -f 1 0x77 0x03 0x00  # G
i2cset -y -f 1 0x77 0x04 0xff  # B
```

#### Turn off the system in 20 seconds, with a blinking LED until the system has been stopped
```
i2cset -y -f 1 0x77 0x01 2     # mode: blinking
i2cset -y -f 1 0x77 0x02 0xff  # R
i2cset -y -f 1 0x77 0x03 0x00  # G
i2cset -y -f 1 0x77 0x04 0x00  # B
i2cset -y -f 1 0x77 0x09 20    # power off timer
```


