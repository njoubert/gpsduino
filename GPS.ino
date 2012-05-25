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



#include "config.h"
#include "StatusFSM.h"
#include "ButtonInterrupt.h"
#include "FeedGPS.h"
#include "FeedSD.h"


void setup()
{
  Serial.begin(115200);
  Serial.println("\n=== GPS Datalogger ===");

  StatusFSM::instance().setup();

  FSD_setupSD();

  FGPS_setupGPS();

  BI_setupButtonInterrupt();
  
  Serial.flush();
  
}



bool INTERACTIVE_MODE = false;

void interactive_loop() {
  Serial.println("======================");
  Serial.println("Entering Interactive Mode...");
  while(true) {
    
    StatusFSM::instance().tick();
    
    if (Serial.available() > 0) {
      int incomingByte = Serial.read();
      
      if (incomingByte == 'I') {
        break;
      }
      if (incomingByte == 'D') {
        FSD_dumpSD();
      }
      if (incomingByte == 'S') {
        StatusFSM::instance().dumpStatus();
      }
    }
    
  }
  Serial.println("Leaving Interactive Mode...");
  Serial.println("======================");
}

/* Here is the pipeline */
void loop() {

  if (Serial.available() > 0) {
    int incomingByte = Serial.read();
    if (incomingByte == 'I') {
      interactive_loop();
    }
  } 
  

  //Stage 1:
  bool gpsHasNewData = FGPS_feedGPS();

  //Stage 2:
  FSD_feedSD(gpsHasNewData, BI_isButtonPressed(), FGPS_getGPSModule());

  //Reset button press state:
  BI_resetButtonPressed();

  //Stage 3:
  StatusFSM::instance().tick();
  

}

