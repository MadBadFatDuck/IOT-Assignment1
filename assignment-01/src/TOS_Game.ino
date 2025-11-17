#include "hardware.h" 
#include "game.h"

void setup() {
  
  setupHardware(); 
  initializeGame();
}

void loop() {
  int pin1_state = getButtonState(PIN1);
  int pin2_state = getButtonState(PIN2);
  int pin3_state = getButtonState(PIN3);
  int pin4_state = getButtonState(PIN4);

  int sensor_value = readPotentiometer(); 
  
  updateGame(pin1_state, pin2_state, pin3_state, pin4_state, sensor_value);  
}