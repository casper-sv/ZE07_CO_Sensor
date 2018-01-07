/*
  ZE07CO_Sensor.h - ZE07-CO Sensor library
  Developed by Valentin - 2018/01/03

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.



Version 1.0: 03 Jan 2018 by Valentin
- Updated to build against Arduino 1.0.6
- Made accessors inline in the header so they can be optimized away
*/


#ifndef __ZE07CO_Sensor_H_
#define __ZE07CO_Sensor_H_

#include <SoftwareSerial.h>
#include <arduino.h>

#define MAXLENGTH  9

class ZE07CO_Sensor
{
  public:
  ZE07CO_Sensor(HardwareSerial* Serial);	//read the uart signal by hardware uart,such as D0
  ZE07CO_Sensor(SoftwareSerial* Serial);	//read the uart signal by software uart,such as D10
  ZE07CO_Sensor(int pin,float ref);			//read the analog signal by analog input pin ,such as A2; ref:voltage on AREF pin
  boolean available(uint16_t timeout);		//new data was recevied
  float uartReadPPM();		//get the concentration(ppm) by uart signal
  float dacReadPPM();		//get the concentration(ppm) by analog signal

  private:
  Stream *mySerial;
  byte receivedCommandStack[MAXLENGTH];
  byte checkSum(byte array[],byte length);
  void boucle();
  boolean receivedFlag;
  byte _sensorPin;
  enum STAT_ZE07 { STATUS_WAITING, STATUS_OK };	
  STAT_ZE07 _status;
  uint8_t _index = 0;
  float _ref;
};

#endif