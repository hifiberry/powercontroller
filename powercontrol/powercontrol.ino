/*

*/
#include <Wire.h>
#include <EEPROM.h>

#include "FR_RotaryEncoder.h"

#define LEDB                      PIN_PD0
#define LEDG                      PIN_PD1
#define LEDR                      PIN_PD2

#define ENCODERA                  PIN_PC0
#define ENCODERB                  PIN_PC1
#define ROTARYSWITCH              PIN_PC2

#define INTERRUPT_PIN             PIN_PA1
#define POWERSWITCH_PIN           PIN_PA0

#define I2CADDR                   7

#define REG_NONE                  0x00 // used for store/restore
#define REG_LEDMODE               0x01
#define REG_LEDR                  0x02
#define REG_LEDG                  0x03
#define REG_LEDB                  0x04
#define REG_BUTTONMODE            0x05
#define REG_BUTTONSTATE           0x06
#define REG_BUTTONDURATION        0x07
#define REG_POWERSWITCH           0x08
#define REG_POWEROFFTIMER         0x09
#define REG_BUTTONPOWEROFFTIMER   0x0a
#define REG_ROTARYVALUE           0x0b
#define REG_ROTARYCHANGE          0x0c
#define REG_WATCHDOGSECONDS       0x0d  // toggle power if this goes down to 0
#define REG_STORE                 0x0e
#define REG_RESTORE               0x0f
#define REG_OVERFLOW              0x10  // unused

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


unsigned int tmp = 0;
unsigned int Aold = 0;
unsigned int Bnew = 0;

volatile byte i2c_register = 0;
volatile byte regs[MAXREG+1];

unsigned long shutdowntime = 0;

RotaryEncoder rotary(ENCODERA, ENCODERB, ROTARYSWITCH);

void toggle(int pin) {
  digitalWrite(pin, !digitalRead(pin));
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveI2C(int bytes) {
  // Read the first byte to determine which register is concerned
  i2c_register = Wire.read();

  if (i2c_register > MAXREG) {
    i2c_register = MAXREG;
  }

  // If there are more than 1 byte, then the master is writing to the slave
  if (bytes > 1) {
    regs[i2c_register] = Wire.read();
  }
}


void sendI2C() {
  // Read from the register variable to know what to send back
  Wire.write(regs[i2c_register]);

  // some registers are cleaned after a read
  if (i2c_register == REG_ROTARYCHANGE) {
    regs[i2c_register]=0;
  }
}

void notify() {
  // toggle notify pin
  toggle(INTERRUPT_PIN);
}

// Interrupt handling routine for the rotary
void ISRrotary() {
   rotary.rotaryUpdate();
   regs[REG_ROTARYVALUE]=rotary.getPosition() && 0xff;
   toggle(LEDR);
}

// Interrupt handling routine for the switch
void ISRswitch() {
  rotary.switchUpdate();
  // always power on if the switch is pressed
   
  regs[REG_POWERSWITCH]=1;
}


void setup() {
  // Power pin
  regs[REG_POWERSWITCH] = 0;
  pinMode(POWERSWITCH_PIN, OUTPUT);
  digitalWrite(POWERSWITCH_PIN,0);

  // LED pins
  pinMode(LEDB, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDR, OUTPUT);


  // LED selftest
  digitalWrite(LEDR,1);
  digitalWrite(LEDG,1);
  digitalWrite(LEDB,1);
  
  digitalWrite(LEDR,0);
  delay(200);
  digitalWrite(LEDR,1);
  digitalWrite(LEDG,0);
  delay(200);
  digitalWrite(LEDG,1);
  digitalWrite(LEDB,0);
  delay(200);
  digitalWrite(LEDB,1);


  // Configure rotary + switch
  rotary.setRotaryLimits(-32767, 32768, true);
  attachInterrupt(ENCODERA, ISRrotary, CHANGE);
  attachInterrupt(ENCODERB, ISRrotary, CHANGE);
  attachInterrupt(ROTARYSWITCH, ISRswitch, CHANGE);

  // Configure I2S
  Wire.begin(I2CADDR);
  Wire.onReceive(receiveI2C);
  Wire.onRequest(sendI2C);

  // interrupt pin
  pinMode(INTERRUPT_PIN, OUTPUT);

  // read register
  //for (int i=0; i<MAXREG; i++) {
  //  EEPROM.get(regs[i], i);
  //}

  // default registers
  regs[REG_ROTARYVALUE]=128;

  //
}

int epOld = 0;
int ep = 0;

int led_loop = 0;
int multiplier = 0;


int pulsing() {
  led_loop += 1;
  int p = led_loop & 0xff;
  if (p>0x7f) {
    p=0xff-p;
  }
  return p;
}

int blink() {
  led_loop += 1;
  int p = led_loop & 0xff;
  if (p>0x7f) {
    return 0;
  } else {
    return 127;
  }
}

int flash() {
  led_loop += 1;
  int p = led_loop & 0xff;
  if (p<2) {
    return 127;
  } else {
    return 0;
  }
}

void loop() {
  if (regs[REG_POWERSWITCH]) {
    digitalWrite(POWERSWITCH_PIN,1);
    if (regs[REG_LEDMODE] == LEDMODE_STATIC) {
      multiplier = 127;
    } else if (regs[REG_LEDMODE] == LEDMODE_PULSING) {
      multiplier = pulsing();
    } else if (regs[REG_LEDMODE] == LEDMODE_BLINK) {
      multiplier = blink();
    } else if (regs[REG_LEDMODE] == LEDMODE_FLASH) {
      multiplier = flash();
    }
    analogWrite(LEDR, 255-(regs[REG_LEDR]*multiplier)/128);
    analogWrite(LEDG, 255-(regs[REG_LEDG]*multiplier)/128);
    analogWrite(LEDB, 255-(regs[REG_LEDB]*multiplier)/128);
  } else {
    digitalWrite(LEDR, 1);
    digitalWrite(LEDG, 1);
    digitalWrite(LEDB, 1);
    digitalWrite(POWERSWITCH_PIN,0);
    delay(100);
  }

  // restore a register from EEPROM?
  if (regs[REG_POWEROFFTIMER]

  // check if the system should be turned off
  if (regs[REG_POWEROFFTIMER]) {
    if (!(shutdowntime)) {
      shutdowntime = millis()+1000*regs[REG_POWEROFFTIMER];
    }
  } else {
    shutdowntime=0;
  }
  if (shutdowntime && shutdowntime<millis()) {
    shutdowntime=0;
    led_loop = 0;
    regs[REG_POWEROFFTIMER]=0;
    regs[REG_POWERSWITCH]=0;
  }
  delay(20);
}
