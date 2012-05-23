/*
  GPS Entrypoint
 */

// Definition of interrupt names
// ISR interrupt service routine
#include <avr/io.h>
#include <avr/interrupt.h>

#include <SoftwareSerial.h>
#include <SD.h>
#include "TinyGPS.h"
#include "tsip.h"


#define BUTTON_DEBOUNCE_DELAY 200

const int buttonPin =   3;


const int gpsRxPinA = 5;
const int gpsTxPinA = 6;

const int gpsRxPinB = 7;
const int gpsTxPinB = 8;

const int chipSelect = 4;

SoftwareSerial nmeaSerial(gpsRxPinB, gpsTxPinB);
SoftwareSerial tsipSerial(gpsRxPinA, gpsTxPinA);
TinyGPS gps;

boolean saveToSD = false;


long previousMillis = 0;
unsigned long MIN_PRINT_TIME = 1000;
void dumpGps() {
  
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis <= MIN_PRINT_TIME) {
    return;
  } else {
    
    previousMillis = currentMillis;
    
    Serial.println("\nGOTLOC");
  
    // unsigned long ch;
    // unsigned short go;
    // unsigned short ba;
    // gps.stats(&ch,&go,&ba);
    // Serial.println(ch);
    // Serial.println(go);
    // Serial.println(ba);
  
  
    Serial.println(gps.altitude());
    Serial.println(gps.speed());
    Serial.println(gps.course());
    unsigned long date;
    unsigned long curtime;
    unsigned long age;
    gps.get_datetime(&date,&curtime,&age);
    Serial.println(date);
    Serial.println(curtime);
  
    long lat;
    long lon;
    gps.get_position(&lat,&lon);
    Serial.println(lat);
    Serial.println(lon);
  
  }
  

}

class StatusFSM {
  
  #define LED_TOP_PIN     2
  #define LED_LEFT_PIN   10
  #define LED_MID_PIN     9
  #define LED_RIGHT_PIN   8
  
  static const int leds[4] = {LED_TOP_PIN,LED_LEFT_PIN,LED_MID_PIN,LED_RIGHT_PIN};
  static const int pinState[4] = {LOW,LOW,LOW,LOW};
  
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
  
  
};

StatusFSM status;

volatile long lastDebounceTime = 0;

volatile bool stickyButtonPressed = false;

void button_interrupt() {
  long now = millis();
  if ((now - lastDebounceTime) > BUTTON_DEBOUNCE_DELAY) {
    stickyButtonPressed = true;
  }
  lastDebounceTime = now;
  
}

void setup()
{
  Serial.begin(57600);
  Serial.println("\n=== GPS Datalogger ===");

  Serial.print("Initializing UI...");
    pinMode(buttonPin, INPUT); 
    pinMode(topLEDpin  , OUTPUT); 
    pinMode(leftLEDpin , OUTPUT); 
    pinMode(midLEDpin  , OUTPUT); 
    pinMode(rightLEDpin, OUTPUT);

	attachInterrupt(1,button_interrupt,RISING);

  Serial.println("DONE");

  Serial.print("Initializing SD card...");
    if (!SD.begin(chipSelect)) {
      Serial.println("FAILED");
    } else {
      Serial.println("READY");
      saveToSD = true;
    }


  Serial.print("Initializing GPS for NMEA...");
    tsipSerial.begin(9600);
    nmeaSerial.begin(4800);
    //tsipSetNMEA(&tsipSerial,TRIMBLE_NMEA_GGA | TRIMBLE_NMEA_RMC);
    //tsipSaveToFLASH(&tsipSerial);
  Serial.println("READY");
  
  digitalWrite(topLEDpin  ,LOW);
  digitalWrite(leftLEDpin ,LOW);
  digitalWrite(midLEDpin  ,LOW);
  digitalWrite(rightLEDpin,LOW);
  delay(500);
  digitalWrite(topLEDpin  ,HIGH);
  digitalWrite(leftLEDpin ,HIGH);
  digitalWrite(midLEDpin  ,HIGH);
  digitalWrite(rightLEDpin,HIGH);
  delay(200);
  digitalWrite(topLEDpin  ,LOW);
  digitalWrite(leftLEDpin ,LOW);
  digitalWrite(midLEDpin  ,LOW);
  digitalWrite(rightLEDpin,LOW);

  
}



void loop() {
  digitalWrite(leftLEDpin, LOW);
  digitalWrite(midLEDpin, LOW);
  digitalWrite(rightLEDpin, LOW);
  digitalWrite(topLEDpin, LOW);
  
  if (stickyButtonPressed) {
    Serial.println("PRESS");
    stickyButtonPressed = false;
  }
  
//   while (nmeaSerial.available()) {
//     int c = nmeaSerial.read();
// 
//     if (gps.encode(c)) {
// //      dumpGps();
//        
// 
//     }
//   }
    


  // } else {
  // 
  //   if (nmeaSerial.available()) {
  //     uint8_t c = nmeaSerial.read();
  // 
  //     File dataFile = SD.open("datalog.txt", FILE_WRITE);
  //     if (saveToSD && dataFile) {
  //       dataFile.print(dataString);
  //       dataFile.close();
  //     }  
  // 
  //     Serial.write(c);
  //    }
  //   }

}

