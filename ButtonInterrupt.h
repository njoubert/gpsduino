#ifndef __ButtonInterrupt_h_
#define __ButtonInterrupt_h_

#include "config.h"
#include "StatusFSM.h"
#include <Arduino.h>

volatile long lastDebounceTime = 0;
volatile bool stickyButtonPressed = false;

void BI_button_interrupt_routine() {
  long now = millis();
  if ((now - lastDebounceTime) > BUTTON_DEBOUNCE_DELAY) {
    stickyButtonPressed = true;
    StatusFSM::instance().pushed();
#ifdef MIRROR_BUTTON_PRESS_INTERRUPT
    Serial.println("INTERRUPT: Button Press Fired and Debounced.");
#endif
  }
  lastDebounceTime = now;
  
}

inline bool BI_isButtonPressed() {
  return stickyButtonPressed;
}
inline void BI_resetButtonPressed() {
  stickyButtonPressed = false;
}

void BI_setupButtonInterrupt() {
  Serial.print("Initializing Button UI...");
  pinMode(BUTTON_PIN, INPUT);
  attachInterrupt(1,BI_button_interrupt_routine,RISING);  
  Serial.println("DONE");
  
}

/* __ButtonInterrupt_h_ */
#endif