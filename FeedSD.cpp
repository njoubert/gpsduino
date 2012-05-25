#include "FeedSD.h"


File navFile;
unsigned long fileDate;
bool saveToSD = false;

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

void SD_write_trackpoint(TinyGPS& gps) {
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

void printFile(File file) {
  //while(file.available()) {
//    Serial.write(file.read());
  //}
}

void printDirectory(File dir) {
  while(true) {

    File entry =  dir.openNextFile();
    if (!entry) {
      return;
    }
    Serial.println(entry.name());
    if (!entry.isDirectory() && entry.name()[0] != '.') {
      printFile(entry);
    }
  }
}



/*


  PUBLICALLY VISIBLE

*/

long last_sd_feed = 0;

void FSD_setupSD() {
  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CHIP_SELECT)) {
    StatusFSM::instance().sd_unknown();
    Serial.println("FAILED");
  } else {
    StatusFSM::instance().sd_good();
    Serial.println("READY");
    saveToSD = true;
  }
}

/* Stage 2 of the pipeline */
void FSD_feedSD(bool gpsHasNewData, bool buttonPressed, TinyGPS& gps) {
  if (!(gpsHasNewData || buttonPressed)) {
    return;
  }
  
  long now = millis();

  if (!buttonPressed && (now - last_sd_feed < SD_FEED_MIN_DUR))
    return;
  else if (!buttonPressed)
    last_sd_feed = now;
    

  if (gpsHasNewData) {
    //SD_write_trackpoint(gps);
  }
  if (buttonPressed) {
    Serial.println("Feeding the SD with button press");
    //dumpGps(gps);
  }
}

void FSD_dumpSD() {
  if (navFile) {
    navFile.close();
  }
  
  File root = SD.open("/");
  Serial.println("Dumping SD contents...");
  printDirectory(root);
  
  
}
