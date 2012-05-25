#ifndef __StatusFSM_h_
#define __StatusFSM_h_

#define SET_ALL_GOOD do { all_good = ((gps_status == GOOD) && (sd_status == GOOD)); } while(0);

class StatusFSM {

  #define LED_TOP_PIN     2
  #define LED_LEFT_PIN   10
  #define LED_MID_PIN     9
  #define LED_RIGHT_PIN   8

  #define OK_LED     0
  #define BUTTON_LED 1
  #define GPS_LED    2
  #define SD_LED     3
  
  #define FAST_BLINK_DELAY 100
  #define BUTTON_BLINK_DELAY 500
  #define SLOW_BLINK_DELAY_ON 100
  #define SLOW_BLINK_DELAY_OFF 1000
    
private:
  
  long last_blink_switch;
  long last_slow_blink_switch;
  bool slow_blink_state;
  
  int leds[4];
  int pinState[4];
  
  
  
  bool button_pressed;
  bool button_blink_in_progress;
  long button_blink_start_time;
  
  bool sd_write;
  bool sd_write_in_progress;
  long sd_write_start_time;
  
  enum STATUS { UNKNOWN=0, BAD=1, GOOD=2 };

  unsigned int gps_status;
  unsigned int sd_status;
  bool all_good;
  
public:

  
  static StatusFSM& instance() {
    static StatusFSM s;
    return s;
  }
    
  void setup() {
    pinMode(LED_TOP_PIN  , OUTPUT); 
    pinMode(LED_LEFT_PIN , OUTPUT); 
    pinMode(LED_MID_PIN  , OUTPUT); 
    pinMode(LED_RIGHT_PIN, OUTPUT);
    
    setAll(LOW);
    digitalWriteAll();
    delay(500);
    setAll(HIGH);
    digitalWriteAll();
    delay(500);
    setAll(LOW);
    digitalWriteAll();
  }
  
  void tick() {
    long now = millis();
    
    if (all_good) {
      slowBlink(OK_LED);
    } else {
      setPin(OK_LED, LOW);      
    }
    
    if (button_pressed) {
      blinkPinOnce(BUTTON_LED, now, button_pressed, button_blink_in_progress, button_blink_start_time, BUTTON_BLINK_DELAY);
    } else {
      setPin(BUTTON_LED, LOW);
    }
    
    switch (gps_status) {
      case UNKNOWN:
      setPin(GPS_LED,HIGH);
      break;
      case BAD:
      fastBlink(GPS_LED,now);
      break;
      case GOOD:
      setPin(GPS_LED,LOW);
      break;
      default:
      Serial.println("Gps Status INTERNAL ERROR");
      break;
    }
    
    switch (sd_status) {
      case UNKNOWN:
      setPin(SD_LED,HIGH);
      break;
      case BAD:
      fastBlink(SD_LED,now);
      break;
      case GOOD:
      setPin(SD_LED,LOW);
      break;
      default:
      Serial.println("SD Status INTERNAL ERROR");
      break;
    } 

    if (sd_write) {
      blinkPinOnce(SD_LED, now, sd_write, sd_write_in_progress, sd_write_start_time, FAST_BLINK_DELAY);
    }
    
    updateFastBlink(now);
    updateSlowBlink(now);
    digitalWriteAll();
  }
  
  void dumpStatus() {
    Serial.print("GPS_STATUS: ");
    if (gps_status == UNKNOWN)
      Serial.println("unknown");
    else if (gps_status == BAD)
      Serial.println("bad");
    else if (gps_status == GOOD)
      Serial.println("good");
    else
      Serial.println("***INTERNAL ERROR***");
    
    Serial.print("SD_STATUS: ");
    if (sd_status == UNKNOWN)
      Serial.println("unknown");
    else if (sd_status == BAD)
      Serial.println("bad");
    else if (sd_status == GOOD)
      Serial.println("good");
    else
      Serial.println("***INTERNAL ERROR***");
    
    Serial.print("ALL_GOOD: ");
    if (all_good)
      Serial.println("yes");
    else
      Serial.println("no");
    
  }
  
  void gps_unknown() { gps_status = UNKNOWN; SET_ALL_GOOD }
  void gps_bad()     { gps_status = BAD; SET_ALL_GOOD }
  void gps_good()    { gps_status = GOOD; SET_ALL_GOOD }
  
  void sd_unknown()  { sd_status = UNKNOWN; SET_ALL_GOOD }
  void sd_bad()      { sd_status = BAD; SET_ALL_GOOD }
  void sd_good()     { sd_status = GOOD; SET_ALL_GOOD }
  
  void pushed() { button_pressed = true; }  
  void write_sd() { sd_write = true; }
  
private:
  StatusFSM() {
    leds[0] = LED_TOP_PIN;
    leds[1] = LED_LEFT_PIN;
    leds[2] = LED_MID_PIN;
    leds[3] = LED_RIGHT_PIN;
    
    for (unsigned int i = 0; i < 4; i++) {
      pinState[i] = LOW;
    }
    gps_status = UNKNOWN;
    sd_status  = UNKNOWN;
    all_good = false;
    last_blink_switch = 0;
    last_slow_blink_switch = 0;
    slow_blink_state = false;
    button_pressed = false; 
  };

  
  inline void setAll(int state) {
    setPin(0, state);
    setPin(1, state);
    setPin(2, state);
    setPin(3, state);
  }

  inline void setPin(int pin, int state) {
    pinState[pin] = state;
  }

  inline void digitalWriteAll() {
    for (unsigned int i = 0; i < 4; i++) {
      digitalWrite(leds[i],pinState[i]);
    }    
  }

  inline void swapPin(int pin) {
    if (pinState[pin] == HIGH) {
      setPin(pin, LOW);
    } else {
      setPin(pin, HIGH);
    }
  }
  
  inline void fastBlink(int pin, long currentTime) {
    if (currentTime - last_blink_switch > FAST_BLINK_DELAY) {
      swapPin(pin);
    }
  }
  inline void updateFastBlink(long currentTime) {
    if (currentTime - last_blink_switch > FAST_BLINK_DELAY) {
      last_blink_switch = currentTime;
    }
  }

  inline void slowBlink(int pin) {
    if (slow_blink_state)
      setPin(pin, HIGH);
    else
      setPin(pin, LOW);    
  }
  inline void updateSlowBlink(long currentTime) {
    if (!slow_blink_state && (currentTime - last_slow_blink_switch > SLOW_BLINK_DELAY_OFF)) {
      slow_blink_state = true;
      last_slow_blink_switch = currentTime;
    } else if (slow_blink_state && (currentTime - last_slow_blink_switch > SLOW_BLINK_DELAY_ON)) {
      slow_blink_state = false;
      last_slow_blink_switch = currentTime;
    }
  }

  inline void blinkPinOnce(int pin, long now, bool & flip, bool & in_progress, long & start, long delay) {
    if (!in_progress) {
      start = now;
      in_progress = true;
    }
    if (now - start > delay) {
      in_progress = false;
      flip = false;
    }
    setPin(pin, HIGH);
  }
  
  #undef LED_TOP_PIN
  #undef LED_LEFT_PIN
  #undef LED_MID_PIN
  #undef LED_RIGHT_PIN
};

/* __StatusFSM_h_ */
#endif 
