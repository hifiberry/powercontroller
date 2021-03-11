#define REG_NONE                  0x00 // used for store/restore
#define REG_LEDMODE               0x01 // R/W - static, pulsing, blinking, ...
#define REG_LEDR                  0x02 // R/W - LED red brightness
#define REG_LEDG                  0x03 // R/W - LED green brightness
#define REG_LEDB                  0x04 // R/W - LED blue brightness
#define REG_BUTTONMODE            0x05 // R/W - report short/long press or press/release (not yet implemented)
#define REG_BUTTONSTATE           0x06 // R   - current state of the button
#define REG_BUTTONDURATION        0x07 // R   - duration of the last button press (not yet implemented)
#define REG_POWERSWITCH           0x08 // R/W - state of the power switch (0=off)
#define REG_POWEROFFTIMER         0x09 // R/W - power off timer in seconds
#define REG_BUTTONPOWEROFFTIME    0x0a // R/W - how long must the button be pressed to power off the system
#define REG_ROTARYVALUE           0x0b // R/W - current state of the rotary control
#define REG_ROTARYCHANGE          0x0c // R/W - last change of the rotary (will be set to 0 after reading the register)

// put some stuff at the end of the table that might grow with the time
#define REG_STORE                 0xfb // W   - store register set to EEPROM (not yet implemented)
#define REG_RESTORE               0x0c // W   - restore register set from EEPROM
#define REG_VERSION_LSB           0xfd // R   - program version LSB
#define REG_VERSION_MSB           0xfe // R   - program version MSB

#define REG_OVERFLOW              0xff  // unused
#define MAXREG                    REG_OVERFLOW

// LED control
#define LEDMODE_STATIC            0
#define LEDMODE_PULSING           1
#define LEDMODE_BLINK             2
#define LEDMODE_FLASH             3

// Button 
#define BUTTONMODE_SHORT_LONG_PRESS   0
#define BUTTONMODE_PRESS_RELEASE      1
#define BUTTON_SHORTPRESS             0
#define BUTTON_LONGPRESS              1
#define BUTTON_PRESS                  2
#define BUTTON_RELEASE                3
