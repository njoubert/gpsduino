/*
  GPS Entrypoint
 */

#include <SoftwareSerial.h>
#include <SD.h>
#include "TinyGPS.h"

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

const int useNMEA = true;

const int chipSelect = 4;

SoftwareSerial nmeaSerial(gpsRxPinB, gpsTxPinB);
SoftwareSerial tsipSerial(gpsRxPinA, gpsTxPinA);
TinyGPS gps;

boolean saveToSD = false;



void readPacket() {
  tsipSerial.read(); //get rid of DLE
  if (tsipSerial.peek() == ETX) {
    tsipSerial.read();      //woops, we are already at the end? skip this...
  } else {
    uint8_t id = tsipSerial.read();
    Serial.print("We read a TSImP packet with ID ");
    Serial.println(id, HEX);
  }  

}

void tsipSetNMEA() {

    //This builds a packet that forces ZDA, VTG and GGA commands:  
    uint8_t id = 0x7A;
    uint8_t a = 0x00;
    uint8_t b = 0x01;
    uint8_t c = 0x00;
    uint8_t d = 0x00;
    uint8_t e = 0x00;
    uint8_t f = 0x25;

    tsipSerial.write(DLE);
    tsipSerial.write(id);
    tsipSerial.write(a);
    tsipSerial.write(b);
    tsipSerial.write(c);
    tsipSerial.write(d);
    tsipSerial.write(e);
    tsipSerial.write(f);
    tsipSerial.write(DLE);
    tsipSerial.write(ETX);
    tsipSerial.flush();
}

void tsipSaveToFLASH() {
    //This builds a packet that forces ZDA, VTG and GGA commands:  
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

  pinMode(buttonPin, INPUT); 
  pinMode(topLEDpin, OUTPUT); 
  pinMode(leftLEDpin, OUTPUT); 
  pinMode(midLEDpin, OUTPUT); 
  pinMode(rightLEDpin, OUTPUT); 



  Serial.begin(9600);
  Serial.println("\n=== GPS Datalogger ===");

  if (!SD.begin(chipSelect)) {
    Serial.println("SD Card failed, or not present...");
  } else {
    saveToSD = true;
  }

  if (useNMEA) {
    Serial.println("Initializing GPS for NMEA...");
    tsipSerial.begin(9600);
    nmeaSerial.begin(4800);
    tsipSetNMEA();
    //tsipSaveToFLASH();

  } else {
    Serial.println("Initializing GPS for TSIP...");
    tsipSerial.begin(9600);
  }
}

void loop()
{


  digitalWrite(topLEDpin, HIGH); 
  digitalWrite(leftLEDpin, LOW); 
  digitalWrite(midLEDpin, LOW); 
  digitalWrite(rightLEDpin, LOW); 

  if (useNMEA) {

    if (true) {

      while (nmeaSerial.available()) {
        int c = nmeaSerial.read();
        Serial.print((char)c);
        if (gps.encode(c)) {

          long latitude;
          long longitude;
          unsigned long fix_age;
          unsigned long date;
          unsigned long time;

          gps.get_position(&latitude,&longitude,&fix_age);
          gps.get_datetime(&date, &time);

          if (fix_age == TinyGPS::GPS_INVALID_AGE) {
            Serial.println("Invalid fix..."); 
          } else {
            Serial.print("Lat: ");
            Serial.print(latitude);
            Serial.print(" Long: ");
            Serial.print(longitude);
            Serial.print(" Date: ");
            Serial.print(date);
            Serial.print(" Time: ");
            Serial.println(time);

          }

          //new data available,
        }  
      }



    } else {

    if (nmeaSerial.available()) {
      uint8_t c = nmeaSerial.read();

      File dataFile = SD.open("datalog.txt", FILE_WRITE);
      if (saveToSD && dataFile) {
        dataFile.print(dataString);
        dataFile.close();
      }  

      Serial.write(c);
     }
    }
  } else {



    if (tsipSerial.available()) {
      uint8_t c = tsipSerial.read();
      if (c<0x10) {Serial.print("0");}
      Serial.println(c, HEX);


      //if (tsipSerial.peek() == DLE) {
      //  readPacket();
      //} else {
      //  //ignore, seek on for next character...
      //  tsipSerial.read();
      //}
    }



  }
}

