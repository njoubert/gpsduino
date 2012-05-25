#ifndef __FeedGPS_h_
#define __FeedGPS_h_

#include "config.h"
#include "StatusFSM.h"
#include <SoftwareSerial.h>

#include "tsip.h"
#include "TinyGPS.h"

bool FGPS_setupGPS();

/* Stage 1 of the pipeline */
bool FGPS_feedGPS();

TinyGPS& FGPS_getGPSModule();

/* __FeedGPS_h_ */
#endif