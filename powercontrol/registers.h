#define REG_NONE                  0x00 // used for store/restore
#define REG_LEDMODE               0x01 // R/W - static, pulsing, blinking, ...
#define REG_LEDR                  0x02 // R/W - LED red brightness
#define REG_LEDG                  0x03 // R/W - LED green brightness
#define REG_LEDB                  0x04 // R/W - LED blue brightness
#define REG_BUTTONMODE            0x05 // R/W - report short/long press or press/release (not yet implemented)
#define REG_BUTTONSTATE           0x06 // R/W - current state of the button
#define REG_BUTTONDURATION        0x07 // R   - duration of the last button press (not yet implemented)
#define REG_POWERSWITCH           0x08 // R/W - state of the power switch (0=off)
#define REG_POWEROFFTIMER         0x09 // R/W - power off timer in seconds
#define REG_BUTTONPOWEROFFTIME    0x0a // R/W - how long must the button be pressed to power off the system
#define REG_ROTARYVALUE           0x0b // R/W - current state of the rotary control
#define REG_ROTARYCHANGE          0x0c // R/W - last change of the rotary (will be set to 0 after reading the register)
#define REG_INTERRUPTPIN          0x0e // R/W - what pin is used for the interrupt signal (won't be stored on EEPROM)

// put some stuff at the end of the table that might grow with the time
#define REG_STORE                 0xfb // W   - store register set to EEPROM 
#define REG_RESTORE               0xfc // W   - restore register set from EEPROM
#define REG_VERSION_LSB           0xfd // R   - program version LSB
#define REG_VERSION_MSB           0xfe // R   - program version MSB

// interrupt pins - uses to signal that a button has been pressed or the controller has been rotated
#define INTERRUPT_PIN1            PIN_PA1
#define INTERRUPT_PIN2            PIN_PF0
#define INTERRUPT_PIN3            PIN_PF1

#define REG_OVERFLOW              0xff  // unused
#define MAXREG                    REG_OVERFLOW

// LED control
#define LEDMODE_STATIC                0
#define LEDMODE_PULSING               1
#define LEDMODE_BLINK                 2
#define LEDMODE_FLASH                 3
#define LEDMODE_OFF                   4

// Button 
#define BUTTONMODE_SHORT_LONG_PRESS   0
#define BUTTONMODE_PRESS_RELEASE      1
#define BUTTON_NOTHING                0
#define BUTTON_SHORTPRESS             1
#define BUTTON_LONGPRESS              2
#define BUTTON_PRESS                  3
#define BUTTON_RELEASE                4
