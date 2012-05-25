# GPSDuino

This is a GPS datalogger built around Arduino.

It uses a Trimble Lassen iQ GPS module to read location,
and stores this to a SD card.

## Software

### Building

You can use either the supplied makefile or the Arduino IDE to build this project.

The makefile is configured for the OS X version of Arduino, present at /Applications/Arduino.app

## Hardware



## Design Ideas


### IDEA1: 

Make it a two-stage pipeline.

1. Obviously, the loop() function feeds the TinyGPS. That just keeps happening.

2. Every time TinyGPS reports new info, we report "good gps" to the Status FSM, then we feed the next stage of the pipeline: data writing.

3. This separately reports to a Status FSM, which ONLY reports the status, not anything else.

4. Separately, on button press, we set the flag that it should save a waypoint.

DOWNSIDES:

- What if we want to save a waypoint we have to wait until the next time tinygps says "ok".
- So maybe don't make the two depend on one another. 
- Better safety this way: everything shuld be independent.
- Writing happens periodically, the gps updates periodically, the save-state gets set independently. 

SO...

What do we expect the interaction to be? And what is the corner cases?

- Normal running:
keep receiving data from the gps
write that data to the SD card
remember to rotate files on the SD card...

- Press the button
immediately grab the last known location

- No GPS?
dunno... write data periodically? or write "BAD SIGNAL?"

- No SD?
Simple, just don't to any of the writing

Status FSM:

- Every time you feed the GPS, you need to get the current status from it and report that.



# GPS NMEA

## Examples:

No signal:

    $GPGGA,072322.0,,,,,0,00,,,,,,,*7E
    $GPRMC,072327.00,V,,,,,,,,,,N*7E


Working:
    
    $GPGGA,072328.00,3722.2705,N,12206.6615,W,1,05,1.53,00065,M,-025,M,,*5E
    $GPRMC,072328.00,A,3722.2705,N,12206.6615,W,000.0,000.0,250512,15.6,E,A*1F
