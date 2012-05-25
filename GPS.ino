/*
  GPS Datalogger Entrypoint
  
  
  
  Design:
  
    Currently we have a pipeline design.
    - Zero-th stage checks the current buttonPressed state and reset it
    - First stage queries the trimble GPS for data and feeds the TinyGPS library
    - Second stage queries the SD unit to write data if necessary
    - Third stage ticks the UI to display the appropriate info
    
    We also have interrupts to check for button presses:
    - set a sticky bit if a button press happens
    
  
 */

// Definition of interrupt names
// ISR interrupt service routine
#include <SoftwareSerial.h>
#include <SD.h>

#include "config.h"
#include "TinyGPS.h"
#include "tsip.h"
#include "StatusFSM.h"

SoftwareSerial nmeaSerial(GPS_RX_PIN_B, GPS_TX_PIN_B);
SoftwareSerial tsipSerial(GPS_RX_PIN_A, GPS_TX_PIN_A);
TinyGPS gps;

bool saveToSD = false;


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
  

volatile long lastDebounceTime = 0;
volatile bool stickyButtonPressed = false;
void button_interrupt() {
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

void button_interrupt_config() {
  pinMode(BUTTON_PIN, INPUT);
  attachInterrupt(1,button_interrupt,RISING);  
}


void setup()
{
  Serial.begin(115200);
  Serial.println("\n=== GPS Datalogger ===");

  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CHIP_SELECT)) {
    StatusFSM::instance().sd_unknown();
    Serial.println("FAILED");
  } else {
    StatusFSM::instance().sd_good();
    Serial.println("READY");
    saveToSD = true;
  }


  Serial.print("Initializing GPS for NMEA...");
  tsipSerial.begin(TRIMBLE_TSIP_SERIALSPEED);
  nmeaSerial.begin(TRIMBLE_NMEA_SERIALSPEED);
  tsip_SetNMEA(&tsipSerial,TRIMBLE_NMEA_GGA | TRIMBLE_NMEA_RMC);
  tsip_SetNMEA(&tsipSerial,TRIMBLE_NMEA_GGA | TRIMBLE_NMEA_RMC);
  tsip_SetNMEA(&tsipSerial,TRIMBLE_NMEA_GGA | TRIMBLE_NMEA_RMC);
  #ifdef SAVE_TRIMBLE_CONFIG_TO_FLASH
    tsip_SaveToFLASH(&tsipSerial);
  #endif
  Serial.println("READY");

  
  Serial.print("Initializing Status UI...");
  StatusFSM::instance().setup();
  Serial.println("DONE");


  Serial.print("Initializing Button UI...");
  button_interrupt_config();
  Serial.println("DONE");

  
  Serial.flush();
  
}


void gps_sentence_parsed_callback(bool gps_data_good) {
  if (gps_data_good) {
    StatusFSM::instance().gps_good();
  } else {
    StatusFSM::instance().gps_bad();
  }
}

/* Stage 1 of the pipeline */
inline bool feedGPS() {
  bool newData = false;
  while (nmeaSerial.available()) {
    int c = nmeaSerial.read();
    #ifdef MIRROR_GPS_DATA
    Serial.write(c);
    #endif
    if (gps.encode(c,&gps_sentence_parsed_callback)) {
      //sticky variable
      newData = true;
    }
  }
  return newData;
}



File navFile;
unsigned long fileDate;

void SD_check_file(unsigned long date) {
  if (!navFile || fileDate != date) {
    if (navFile)
      navFile.close();
    
    String filename = String(date);
    char buf[9];
    filename.toCharArray(buf, 9);
    buf[8] = '\0';
    navFile = SD.open(buf, O_APPEND | FILE_WRITE);
  }
}

void SD_write_trackpoint() {
  if (!saveToSD)
    return;
  
  unsigned long date;
  unsigned long curtime;
  unsigned long age;
  gps.get_datetime(&date,&curtime,&age);

  Serial.print(date);
  Serial.print(" ");
  Serial.print(curtime);
  Serial.println(": Feeding the SD with updated GPS data");

  if (age == TinyGPS::GPS_INVALID_AGE || date == TinyGPS::GPS_INVALID_DATE) {
    StatusFSM::instance().gps_bad();
    return;
  }
    
  SD_check_file(date);
  
  if (!navFile) {
    StatusFSM::instance().sd_bad();
  } else {
    
    static const size_t buflen = 20;
    uint8_t buf[buflen];
    static const uint8_t SEP = ',';
    static const uint8_t END = '\n';

    #define WRITE_SD(val) do {          \
      String data = String(val);        \
      data.getBytes(buf,buflen);        \
      buf[data.length()] = '\0';        \
      navFile.write(buf,data.length()); \
      } while(0);
    
    #define WRITE_SD_SEPARATOR navFile.write(&SEP,1)
    #define WRITE_SD_END navFile.write(&END,1);

    long lat;
    long lon;
    gps.get_position(&lat,&lon);

    WRITE_SD(date);
    WRITE_SD_SEPARATOR;
    WRITE_SD(curtime);
    WRITE_SD_SEPARATOR;
    WRITE_SD(age);
    WRITE_SD_SEPARATOR;
    WRITE_SD(lat);
    WRITE_SD_SEPARATOR;
    WRITE_SD(lon);
    WRITE_SD_SEPARATOR;
    
    WRITE_SD(gps.altitude());
    WRITE_SD_SEPARATOR;
    WRITE_SD(gps.speed());
    WRITE_SD_SEPARATOR;
    WRITE_SD(gps.course());
    WRITE_SD_END;
  
    
    navFile.flush();
    StatusFSM::instance().write_sd();

    
  }
  
  
  
}


long last_sd_feed = 0;

/* Stage 2 of the pipeline */
inline void feedSD(bool gpsHasNewData, bool buttonPressed) {
  if (!(gpsHasNewData || buttonPressed)) {
    return;
  }
  
  long now = millis();

  if (!buttonPressed && (now - last_sd_feed < SD_FEED_MIN_DUR))
    return;
  else if (!buttonPressed)
    last_sd_feed = now;
    

  if (gpsHasNewData) {
    SD_write_trackpoint();
  }
  if (buttonPressed) {
    Serial.println("Feeding the SD with button press");
    dumpGps();
  }
}

/* Here is the pipeline */
void loop() {

  //Stage 1:
  bool gpsHasNewData = feedGPS();
  
  //Stage 2:
  feedSD(gpsHasNewData, stickyButtonPressed);
  
  //Reset button press state:
  stickyButtonPressed = false;
  
  //Stage 3:
  StatusFSM::instance().tick();

}

