#ifndef __FeedSD_h_
#define __FeedSD_h_

#include <SD.h>

#include "TinyGPS.h"
#include "config.h"
#include "StatusFSM.h"


void FSD_setupSD();

void FSD_feedSD(bool gpsHasNewData, bool buttonPressed, TinyGPS& gps);

void FSD_dumpSD();

/* __FeedSD_h_ */
#endif