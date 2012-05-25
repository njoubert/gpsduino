#include "FeedGPS.h"

SoftwareSerial nmeaSerial(GPS_RX_PIN_B, GPS_TX_PIN_B);
SoftwareSerial tsipSerial(GPS_RX_PIN_A, GPS_TX_PIN_A);
TinyGPS gps;

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

void gps_sentence_parsed_callback(bool gps_data_good) {
  if (gps_data_good) {
    StatusFSM::instance().gps_good();
  } else {
    StatusFSM::instance().gps_bad();
  }
}

/*

  PUBLICALLY VISIBLE:

 */


bool FGPS_setupGPS() {
  Serial.print("Initializing GPS...");
  
  tsipSerial.begin(TRIMBLE_TSIP_SERIALSPEED);
  nmeaSerial.begin(TRIMBLE_NMEA_SERIALSPEED);
  tsip_SetNMEA(&tsipSerial,TRIMBLE_NMEA_GGA | TRIMBLE_NMEA_RMC);
  tsip_SetNMEA(&tsipSerial,TRIMBLE_NMEA_GGA | TRIMBLE_NMEA_RMC);
  tsip_SetNMEA(&tsipSerial,TRIMBLE_NMEA_GGA | TRIMBLE_NMEA_RMC);
  #ifdef SAVE_TRIMBLE_CONFIG_TO_FLASH
    tsip_SaveToFLASH(&tsipSerial);
  #endif
  
  Serial.println("READY");
}

/* Stage 1 of the pipeline */
bool FGPS_feedGPS() {
  bool newData = false;
  while (nmeaSerial.available()) {
    int c = nmeaSerial.read();
    #ifdef MIRROR_GPS_DATA
    Serial.write(c);
    #endif
    if (gps.encode(c,&gps_sentence_parsed_callback)) {
      newData = true; //sticky variable
    }
  }
  if (newData) {
    Serial.println("New data received from GPS");
  }
  return newData;
}


TinyGPS& FGPS_getGPSModule() {
  return gps;
}




