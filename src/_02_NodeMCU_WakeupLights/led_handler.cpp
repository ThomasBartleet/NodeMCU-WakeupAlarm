#include <Arduino.h>
#include <Ticker.h>  //Ticker Library

#include "led_handler.h"
#include "pinouts.h"

#define FLASH_TIME_PERIOD 2000 // 2000ms per flash cycle

Ticker ledHandler;

LedState currentState;
const int led = WIFI_LED;

bool isLedOn = false;


void _ledIsr();
void _setLedTime(int milliseconds);
uint32_t _getLedTimeOn();
uint32_t _getLedTimeOff();


void initialiseLedHandler() {
  initialiseLedHandler(STATE_IDLE);
}

void initialiseLedHandler(LedState state) {
  pinMode(led, OUTPUT);
  pinMode(BOARD_LED, OUTPUT);
  setBoardLedState(OFF);

  setLedHandlerState(state);

  _ledIsr();
#ifdef DEBUG
  Serial.println("LED Handler initialised.");
#endif
}

void _ledIsr() {
  // LEDs are active low on NodeMCU (?).
  if (isLedOn) {
    // do the switching off
    analogWrite(led, 1023);
    isLedOn = false;
    _setLedTime(_getLedTimeOff());
  } else {
    // do the switching on
    analogWrite(led, 1010);
    isLedOn = true;
    _setLedTime(_getLedTimeOn());
  }
}

void setLedHandlerState(LedState newState) {
  currentState = newState;
#ifdef DEBUG
  Serial.print("LED time on: ");
  Serial.println(_getLedTimeOn());
  Serial.print("LED time off: ");
  Serial.println(_getLedTimeOff());
#endif
}

void _setLedTime(int milliseconds) {
  if (ledHandler.active()) {
    ledHandler.detach();
  }

  ledHandler.once_ms(milliseconds, _ledIsr);
}

uint32_t _getLedTimeOn() {
  switch(currentState) {
    case STATE_FAILED:
    case STATE_IDLE:
      return (uint32_t)(FLASH_TIME_PERIOD / 2);

    case STATE_CONNECTING:
      return 200;

    case STATE_CONNECTED:
      return 50;
  }

  return 0;
}

uint32_t _getLedTimeOff() {
  return FLASH_TIME_PERIOD - _getLedTimeOn();
}

void setBoardLedState(bool enable) {
  digitalWrite(BOARD_LED, enable);
}
