/*
  tsip.h
  
  This defines an interface for the trimble standard interface protocol




*/

#ifndef __tsip_h_
#define __tsip_h_

#include <SoftwareSerial.h>

#define TRIMBLE_TSIP_SERIALSPEED 9600
#define TRIMBLE_NMEA_SERIALSPEED 4800

#define TRIMPLE_TSIP_DLE 0x10
#define TRIMPLE_TSIP_ETX 0x03

#define TRIMBLE_NMEA_RMC  0x0100
#define TRIMBLE_NMEA_TF   0x0200
#define TRIMBLE_NMEA_BA   0x2000
#define TRIMBLE_NMEA_GGA  0x0001
#define TRIMBLE_NMEA_GLL  0x0002
#define TRIMBLE_NMEA_VTG  0x0004
#define TRIMBLE_NMEA_GSV  0x0008
#define TRIMBLE_NMEA_GSA  0x0010
#define TRIMBLE_NMEA_ZDA  0x0020

//This builds a packet that forces the correct NMEA FLAGS to be output:  
void tsip_SetNMEA(SoftwareSerial* serial, uint16_t flags);
  
//This builds a packet that saves the current state to flash rom
void tsip_SaveToFLASH(SoftwareSerial* serial);

/* __tsip_h_ */
#endif 