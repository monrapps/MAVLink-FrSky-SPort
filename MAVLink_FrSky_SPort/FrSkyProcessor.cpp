#include "FrSkyProcessor.h"

// ***********************************************************************
FrSkyProcessor::FrSkyProcessor(const SerialId& serial_pin, uint8_t fault_pin) :
softSerial{   
  serial_pin, serial_pin, true }
, fault_pin{   
  fault_pin }
, crc{     
  0 }
,lastRx{     
  0 }
,gps_index{     
  0 }
,variometer_index{     
  0 }
,vfas_index{     
  0 }
,misc_index{     
  0 }
,serial_id{     
  serial_pin } 
{    
  softSerial.begin(BAUD_RATE);    
}

FrSkyProcessor::~FrSkyProcessor() {
}

void FrSkyProcessor::process(const MavlinkProcessor::MavlinkTelemetry& mav_telemetry, bool new_data) {
  uint8_t data = 0;

  while (softSerial.available())	{
    data = softSerial.read();
    if (lastRx == START_STOP) {
      if (new_data == true)
        digitalWrite(fault_pin, HIGH);
      if (data == SENSOR_ID_VARIO) {
        switch (++variometer_index % variometer_sensor_max) {
        case 0:
          sendPackage(DATA_FRAME, FR_ID_VARIO, mav_telemetry.climb_rate); // OK
          break;
        case 1:
          sendPackage(DATA_FRAME, FR_ID_ALTITUDE, mav_telemetry.bar_altitude * 100); // OK
          break;
        default:
          break; //unreachable
        }
      }
      else if (data == SENSOR_ID_FAS) {
        switch (++vfas_index % vfas_sensor_max) {
        case 0:
          sendPackage(DATA_FRAME, FR_ID_VFAS, mav_telemetry.battery_voltage); // OK
          break;
        case 1:
          sendPackage(DATA_FRAME, FR_ID_CURRENT, mav_telemetry.battery_current); // OK 
          break;
        default:
          break; //unreachable
        }
      }
      else if (data == SENSOR_ID_GPS) {
        uint32_t value;        
        uint32_t temp;
        switch (++gps_index % gps_sensor_max) {
        case 0:
          sendPackage(DATA_FRAME, FR_ID_SPEED, mav_telemetry.gps_speed * 36 / 1000);  // Not Working wrong offset
          break;
        case 1:
          int32_t gpsLatitude;
          gpsLatitude = mav_telemetry.gps_latitude ;		
          value = 0 ;
          if (gpsLatitude < 0)
          {
            value |= 0x40000000 ;
            gpsLatitude = - gpsLatitude ;
          }
          temp = gpsLatitude  * 6 ;
          temp /= 100 ;
          temp &= 0x3FFFFFFF ;
          value |= temp ;
          sendPackage(DATA_FRAME, FR_ID_LATLONG, value);
          break;
        case 2:      
          int32_t gpsLongitude ;
          gpsLongitude = mav_telemetry.gps_longitude ;
          value = 0x80000000 ;
          if (gpsLongitude < 0)
          {
            value |= 0x40000000 ;
            gpsLongitude = - gpsLongitude ;
          }
          temp = gpsLongitude * 6 ;
          temp /= 100 ;
          temp &= 0x3FFFFFFF ;
          value |= temp ;
          sendPackage(DATA_FRAME, FR_ID_LATLONG, value);
          break;
        case 3:
          sendPackage(DATA_FRAME, FR_ID_GPS_ALT, mav_telemetry.gps_altitude / 10); // OK
          break;
        default:
          break; //unreachable
        }
      }
      else if (data == SENSOR_ID_RPM) {
        sendPackage(DATA_FRAME, FR_ID_RPM, mav_telemetry.throttle); // Not Tested 
      }
      else if (data == SENSOR_ID_SP2UH || data == SENSOR_ID_SP2UR) {
        switch (++misc_index % misc_sensor_max) {
        case 0: 
          int16_t hdop_val;
          hdop_val = mav_telemetry.gps_hdop / 4;
          if (hdop_val > 255)
            hdop_val = 255;
          sendPackage(DATA_FRAME, FR_ID_ADC2, hdop_val); // OK 			
          break;        
        case 1:
          sendPackage(DATA_FRAME, FR_ID_HEADING, mav_telemetry.heading * 100); // OK
          break;
        case 2: 
          sendPackage(DATA_FRAME, FR_ID_T1, mav_telemetry.wp_dist); // Not Tested			
          break;
        case 3:
          sendPackage(DATA_FRAME, FR_ID_T2, mav_telemetry.gps_satellites_visible); // OK
          break;
        case 4:
          sendPackage(DATA_FRAME, FR_ID_A3_FIRST, mav_telemetry.gps_speed * 36 / 1000); // OK (GPS SPEED)
          break;
        case 5:
          sendPackage(DATA_FRAME, FR_ID_A4_FIRST, mav_telemetry.battery_remaining); // OK (FUEL)
          break;
        default:
          break;
        }
      }
    }
    lastRx = data;
  }
}

void FrSkyProcessor::sendByte(uint8_t byte) {
  if (byte == 0x7E) {
    softSerial.write(0x7D);
    softSerial.write(0x5E);
  }
  else if (byte == 0x7D) {
    softSerial.write(0x7D);
    softSerial.write(0x5D);
  }
  else {
    softSerial.write(byte);
  }
  //softSerial.write(byte);
  // CRC update
  crc += byte;         //0-1FF
  crc += crc >> 8;   //0-100
  crc &= 0x00ff;
  crc += crc >> 8;   //0-0FF
  crc &= 0x00ff;
}

void FrSkyProcessor::sendCrc() {
  softSerial.write(0xFF - crc);
  crc = 0;          // CRC reset
}

void FrSkyProcessor::sendPackage(uint8_t header, uint16_t id, uint32_t value) {
  pinMode(serial_id, OUTPUT);
  sendByte(header);
  uint8_t *bytes = (uint8_t*)&id;
  sendByte(bytes[0]);
  sendByte(bytes[1]);
  bytes = (uint8_t*)&value;
  sendByte(bytes[0]);
  sendByte(bytes[1]);
  sendByte(bytes[2]);
  sendByte(bytes[3]);
  sendCrc();
  softSerial.flush();
  pinMode(serial_id, INPUT);
  digitalWrite(fault_pin, LOW);
}
