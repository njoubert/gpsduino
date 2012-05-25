#ifndef __StatusFSM_h_
#define __StatusFSM_h_

#include <Arduino.h>
#include "config.h"

#define SET_ALL_GOOD do { all_good = ((gps_status == GOOD) && (sd_status == GOOD)); } while(0);

class StatusFSM {
private:
  
  enum STATUS { UNKNOWN, BAD, GOOD };    
  
  /* State machine: */
  bool button_pressed;
  bool button_blink_in_progress;
  long button_blink_start_time;

  long last_blink_switch;
  long last_slow_blink_switch;
  bool slow_blink_state;
  
  bool sd_write;
  bool sd_write_in_progress;
  long sd_write_start_time;
  

  unsigned int gps_status;
  unsigned int sd_status;
  bool all_good;

  /* Output State: */
  int leds[4];
  int pinState[4];
  
public:
  
  //Singleton pattern
  static StatusFSM& instance() {
    static StatusFSM s;
    return s;
  }
    
  //Run this as part of your Arduino setup() code
  void setup();
  
  //Run this as part of your Arduino loop() code
  void tick();
  
  //Run this to write debug info to Serial
  void dumpStatus();
  
  /* State machine update: */
  inline void gps_unknown() { gps_status = UNKNOWN; SET_ALL_GOOD }
  inline void gps_bad()     { gps_status = BAD; SET_ALL_GOOD }
  inline void gps_good()    { gps_status = GOOD; SET_ALL_GOOD }
  
  inline void sd_unknown()  { sd_status = UNKNOWN; SET_ALL_GOOD }
  inline void sd_bad()      { sd_status = BAD; SET_ALL_GOOD }
  inline void sd_good()     { sd_status = GOOD; SET_ALL_GOOD }
  
  inline void pushed() { button_pressed = true; }  
  inline void write_sd() { sd_write = true; }
  
private:
  StatusFSM();
  
  /* Internals for managing output */
  inline void setAll(int state);
  inline void setPin(int pin, int state);
  void digitalWriteAll();
  void swapPin(int pin);
  
  /* Internals for smart blinking */
  void fastBlink(int pin, long currentTime);
  void updateFastBlink(long currentTime);
  void slowBlink(int pin);
  void updateSlowBlink(long currentTime);
  void blinkPinOnce(int pin, long now, bool & flip, bool & in_progress, long & start, long delay);
  
};

#undef SET_ALL_GOOD

/* __StatusFSM_h_ */
#endif