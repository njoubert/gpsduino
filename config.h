#ifndef __config_h_
#define __config_h_

/* DEBUG DEFINES: */

//#define MIRROR_GPS_DATA
//#define MIRROR_BUTTON_PRESS_INTERRUPT

/* PIN CONFIGURATION */

#define BUTTON_PIN     3

#define SD_CHIP_SELECT 4

#define GPS_RX_PIN_A   5
#define GPS_TX_PIN_A   6
#define GPS_RX_PIN_B   7
#define GPS_TX_PIN_B   8


/* CONFIG */

#define BUTTON_DEBOUNCE_DELAY 500

#define SD_FEED_MIN_DUR 500

//#define SAVE_TRIMBLE_CONFIG_TO_FLASH 


/* StatusFSM config */
#define FAST_BLINK_DELAY 100
#define BUTTON_BLINK_DELAY 500
#define SLOW_BLINK_DELAY_ON 100
#define SLOW_BLINK_DELAY_OFF 1000


/* __config_h_ */
#endif