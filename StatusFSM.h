#ifndef __StatusFSM_h_
#define __StatusFSM_h_


class StatusFSM {

  #define LED_TOP_PIN     2
  #define LED_LEFT_PIN   10
  #define LED_MID_PIN     9
  #define LED_RIGHT_PIN   8

private:
    
  int leds[4];
  int pinState[4];
  
public:
  
  StatusFSM() {
    leds[0] = LED_TOP_PIN;
    leds[1] = LED_LEFT_PIN;
    leds[2] = LED_MID_PIN;
    leds[3] = LED_RIGHT_PIN;
    
    for (unsigned int i = 0; i < 4; i++) {
      pinState[i] = LOW;
    }
  };
  
  void init() {
    pinMode(LED_TOP_PIN  , OUTPUT); 
    pinMode(LED_LEFT_PIN , OUTPUT); 
    pinMode(LED_MID_PIN  , OUTPUT); 
    pinMode(LED_RIGHT_PIN, OUTPUT);
    
    setAll(LOW);
    delay(500);
    setAll(HIGH);
    delay(500);
    setAll(LOW);
  }
  
  void tick() {
    
  }
  // void okay() {
  //   
  // }
  // 
  // void unsetError() {
  //   
  // }
  // 
  // void setError() {
  //   
  // }
  // 
  
private:
  
  inline void setAll(int state) {
    setPin(LED_TOP_PIN  , state);
    setPin(LED_LEFT_PIN , state);
    setPin(LED_MID_PIN  , state);
    setPin(LED_RIGHT_PIN, state);
  }

  inline void setPin(int pin, int state) {
    pinState[pin] = state;
    digitalWrite(pin,state);
  }
  #undef LED_TOP_PIN
  #undef LED_LEFT_PIN
  #undef LED_MID_PIN
  #undef LED_RIGHT_PIN
};

/* __StatusFSM_h_ */
#endif 
