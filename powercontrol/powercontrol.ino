/*

*/
#include <Wire.h>
#include <EEPROM.h>


#include "registers.h"
#include "FR_RotaryEncoder.h"

#define VERSION                   2

#define LEDB                      PIN_PD0
#define LEDG                      PIN_PD1
#define LEDR                      PIN_PD2

#define ENCODERA                  PIN_PC0
#define ENCODERB                  PIN_PC1
#define ROTARYSWITCH              PIN_PC2

#define INTERRUPT_PIN             PIN_PA1
#define POWERSWITCH_PIN           PIN_PA0

#define I2CADDR                   0x77


// Power down long press time in seconds
#define DEFAULT_REG_BUTTONPOWEROFFTIME   5

unsigned int tmp = 0;
unsigned int Aold = 0;
unsigned int Bnew = 0;

volatile byte i2c_register = 0;
volatile byte regs[MAXREG+1];

unsigned long shutdowntime = 0;

int lastSwitchState = 0;
unsigned long startPressed = 0;

RotaryEncoder rotary(ENCODERA, ENCODERB, ROTARYSWITCH);

void toggle(int pin) {
  digitalWrite(pin, !digitalRead(pin));
}


// read register set from EEPROM
void read_eeprom() {
  int regno;
  for (regno=0; regno<MAXREG; regno++) {
    regs[regno]=EEPROM.read(regno);
  }
  
  // version isn't read from EEPROM
  regs[REG_VERSION_LSB] = VERSION && 0xff;
  regs[REG_VERSION_MSB] = (VERSION >> 8) && 0xff;
  
  regs[REG_RESTORE] = 0x00;
}

void write_eeprom() {
  int regno;
  for (regno=0; regno<MAXREG; regno++) {
    EEPROM.write(regno, regs[regno]);
  }
  regs[REG_STORE] = 0;
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
   notify();
}

// Interrupt handling routine for the switch
void ISRswitch() {
  rotary.switchUpdate();
  
  // always power on if the switch is pressed
  if (rotary.getSwitchState() == rotary.SW_ON) {
    regs[REG_POWERSWITCH]=1;
  }

  notify();
}



void setup() {
  // Power pin
  regs[REG_POWERSWITCH] = 1;
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

  // read defaults from EEPROM
  read_eeprom();
}

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

void power_off() {
  regs[REG_POWERSWITCH]=0;
  regs[REG_POWEROFFTIMER]=0;
}

void loop() {
  // 
  // switch polling (not yet fully implemented)
  //
  int currentSwitchState = rotary.getSwitchState();
  if (lastSwitchState != currentSwitchState) {
    lastSwitchState = currentSwitchState;
    if (currentSwitchState == rotary.SW_ON) {
      startPressed = millis();
    }
  } else {
    if (currentSwitchState == rotary.SW_ON) {
      if ( (regs[REG_BUTTONPOWEROFFTIME] > 0) && (startPressed + 1000*regs[REG_BUTTONPOWEROFFTIME] < millis())) {
        power_off();
      } 
    }
  }
  

  //
  // control power state and LED
  //
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
    } else if (regs[REG_LEDMODE] == LEDMODE_OFF) {
      multiplier=0;
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
    power_off();
  }

  // check for EEPROM store/restore commands
  if (regs[REG_STORE] != 0) {
    write_eeprom();
  } 

  if (regs[REG_RESTORE] != 0) {
    read_eeprom();
    regs[1]=0;
  }

  delay(20);
}
