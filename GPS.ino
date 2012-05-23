/*
  GPS Entrypoint
 */

#include <SoftwareSerial.h>
#include <SD.h>
#define _GPS_NO_STATS
#include "TinyGPS.h"
#undef _GPS_NO_STATS

#define DLE 0x10
#define ETX 0x03

const int buttonPin = 3;
const int topLEDpin = 2;
const int leftLEDpin = 10;
const int midLEDpin = 9;
const int rightLEDpin = 8;

const int gpsRxPinA = 5;
const int gpsTxPinA = 6;

const int gpsRxPinB = 7;
const int gpsTxPinB = 8;


const int chipSelect = 4;

SoftwareSerial nmeaSerial(gpsRxPinB, gpsTxPinB);
SoftwareSerial tsipSerial(gpsRxPinA, gpsTxPinA);
TinyGPS gps;

boolean saveToSD = false;

#define TRIMBLE_NMEA_RMC  0x0100
#define TRIMBLE_NMEA_TF   0x0200
#define TRIMBLE_NMEA_BA   0x2000
#define TRIMBLE_NMEA_GGA  0x0001
#define TRIMBLE_NMEA_GLL  0x0002
#define TRIMBLE_NMEA_VTG  0x0004
#define TRIMBLE_NMEA_GSV  0x0008
#define TRIMBLE_NMEA_GSA  0x0010
#define TRIMBLE_NMEA_ZDA  0x0020

void tsipSetNMEA() {
  uint16_t flags = TRIMBLE_NMEA_GGA | TRIMBLE_NMEA_RMC;

  //This builds a packet that forces the correct NMEA FLAGS to be output:  
  uint8_t id = 0x7A;
  
  uint8_t subcode = 0x00;
  uint8_t interval = 0x01;
  uint8_t res = 0x00;
    
  uint8_t flags1 = (uint8_t) (flags >> 8);
  uint8_t flags2 = (uint8_t) flags;
  
  tsipSerial.write(DLE);
  tsipSerial.write(id);
  tsipSerial.write(subcode);
  tsipSerial.write(interval);
  tsipSerial.write(res);
  tsipSerial.write(res);
  tsipSerial.write(flags1);
  tsipSerial.write(flags2);
  tsipSerial.write(DLE);
  tsipSerial.write(ETX);
  tsipSerial.flush();
}

void tsipSaveToFLASH() {
    //This builds a packet that saves the current state to flash rom: 
    uint8_t id = 0x8e;
    uint8_t a = 0x26;

    tsipSerial.write(DLE);
    tsipSerial.write(id);
    tsipSerial.write(a);
    tsipSerial.write(DLE);
    tsipSerial.write(ETX);
    tsipSerial.flush();  
}
String dataString = "HELLO!\n";

void setup()
{
  Serial.begin(57600);
  Serial.println("\n=== GPS Datalogger ===");

  Serial.print("Initializing UI...");
    pinMode(buttonPin, INPUT); 
    pinMode(topLEDpin, OUTPUT); 
    pinMode(leftLEDpin, OUTPUT); 
    pinMode(midLEDpin, OUTPUT); 
    pinMode(rightLEDpin, OUTPUT);
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
    tsipSetNMEA();
    tsipSetNMEA();
    tsipSetNMEA();
    //tsipSaveToFLASH();
  Serial.println("READY");
}


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

long blinkMillis;
int ledState = LOW;
long offInterval = 1500;
long onInterval = 100;

bool flip = false;

void loop() {
  
  // unsigned long currentMillis = millis();
  //  
  // if((ledState == LOW) && (currentMillis - blinkMillis > offInterval)) {
  // 
  //   blinkMillis = currentMillis;   
  //   ledState = HIGH;
  //   digitalWrite(topLEDpin, ledState);
  // } else if ((ledState == HIGH) && (currentMillis - blinkMillis > onInterval)) {
  // 
  //   blinkMillis = currentMillis;   
  //   ledState = LOW;    
  //   digitalWrite(topLEDpin, ledState);
  // 
  // }

  int buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) {  
    flip = !flip;
  }
  if (flip) {
    digitalWrite(leftLEDpin , HIGH);
    digitalWrite(midLEDpin  , HIGH);
    digitalWrite(rightLEDpin, HIGH);
  } else {
    digitalWrite(leftLEDpin , LOW);
    digitalWrite(midLEDpin  , LOW);
    digitalWrite(rightLEDpin, LOW);
    
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

