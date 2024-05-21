# Flashing firmware to the power controller

## Power supply

During the flashing process, the power controller will disable power to the Pi. Therefore, it's important
that during the flashing, power is directly connected to the Pi, not via the power controller.

## Enable serial port

On HiFiBerryOS, you first need to enable the serial port that's used to program the power controller

```
/opt/hifiberry/bin/enable-updi
```

This will disable the onboard Bluetoot controller! To re-enable it after flashing the firmware,
remove the line
```
dtoverlay=disable-bt
```

from /boot/config.txt again.

## Check that the communication with the controller works

### Older HBOS Releases
```
pyupdi -c /dev/ttyAMA0 -d mega808 -fr
```

### HBOS64
```
pymcuprog ping -t uart -d atmega808 -u /dev/ttyAMA0  --uart-timeout 2
```

## Copy the firmware

Just copy the compiled firmware to the Pi using scp. It should be an ".hex" file.

Or just get the firmware from Github:
```
curl -L -o powercontrol.hex https://github.com/hifiberry/powercontroller/releases/download/r5/powercontrol.ino_atmega808_16000000L.hex
```

## Program firmware

### Older HBOS Releases
```
pyupdi -c /dev/ttyAMA0 -d mega808 -f firmware.hex
```

### HBOS64
```
pymcuprog  -t uart -d atmega808 -u /dev/ttyAMA0  --uart-timeout 2 -f powercontrol.hex  write
```
