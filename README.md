# MAVLink FrSky SmartPort

MAVLink Telemetry to FrSky SmartPort converter, designed for Turnigy 9XR (non PRO), using a FrSky XJT Module + X8R reciever (Works on DJT Modules, but not all data are available).

#### IDs

FR_ID_VARIO, mav_telemetry.climb_rate

FR_ID_ALTITUDE, mav_telemetry.bar_altitude * 100

FR_ID_VFAS, mav_telemetry.battery_voltage

FR_ID_CURRENT, mav_telemetry.battery_current

FR_ID_SPEED, mav_telemetry.gps_speed * 36 / 1000

FR_ID_LATLONG, mav_telemetry.gps_latitude (longitude)

FR_ID_GPS_ALT, mav_telemetry.gps_altitude / 10

FR_ID_ADC2, hdop_val

FR_ID_HEADING, mav_telemetry.heading * 100

FR_ID_T1, mav_telemetry.wp_dist)

FR_ID_T2, mav_telemetry.gps_satellites_visible

FR_ID_A3_FIRST, mav_telemetry.gps_speed * 36 / 1000

FR_ID_A4_FIRST, mav_telemetry.battery_remaining

### Modified from:

https://github.com/jociz68/MAVLinkSmartPort


### Using some information from:

http://diydrones.com/forum/topics/amp-to-frsky-x8r-sport-converter

https://github.com/martinbudden/betaflight

https://github.com/cleanflight/cleanflight


## MODs for 9XR

These modifications are only for 9XR (non PRO). The 9XR-PRO does not need this modification, as it already have a Serial Port available on the Futaba connector

### XJT Mod

### DJT Mod
