#include "tsip.h"

//This builds a packet that forces the correct NMEA FLAGS to be output:  
void tsip_SetNMEA(SoftwareSerial* serial, uint16_t flags) {
  uint8_t id = 0x7A;
  
  uint8_t subcode = 0x00;
  uint8_t interval = 0x01;
  uint8_t res = 0x00;
    
  uint8_t flags1 = (uint8_t) (flags >> 8);
  uint8_t flags2 = (uint8_t) flags;
  
  serial->write(TRIMPLE_TSIP_DLE);
  serial->write(id);
  serial->write(subcode);
  serial->write(interval);
  serial->write(res);
  serial->write(res);
  serial->write(flags1);
  serial->write(flags2);
  serial->write(TRIMPLE_TSIP_DLE);
  serial->write(TRIMPLE_TSIP_ETX);
  serial->flush();
}

//This builds a packet that saves the current state to flash rom
void tsip_SaveToFLASH(SoftwareSerial* serial) {
  uint8_t id = 0x8e;
  uint8_t a = 0x26;

  serial->write(TRIMPLE_TSIP_DLE);
  serial->write(id);
  serial->write(a);
  serial->write(TRIMPLE_TSIP_DLE);
  serial->write(TRIMPLE_TSIP_ETX);
  serial->flush();  
}