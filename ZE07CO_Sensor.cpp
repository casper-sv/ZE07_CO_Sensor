/*
  ZE07CO_Sensor.cpp - ZE07-CO_Sensor library
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

#include "ZE07CO_Sensor.h"

ZE07CO_Sensor::ZE07CO_Sensor(HardwareSerial* Serial)	//read the uart signal by hardware uart,such as D0
{
    mySerial = Serial;
    receivedFlag = 0;
}

ZE07CO_Sensor::ZE07CO_Sensor(SoftwareSerial* Serial)	//read the uart signal by software uart,such as D10
{
    mySerial = Serial;
    receivedFlag = 0;
}
ZE07CO_Sensor:: ZE07CO_Sensor(int pin,float ref)//read the analog signal by analog input pin ,such as A2; ref:voltage on AREF pin
{
    _sensorPin = pin;
    _ref = ref;											//for arduino uno ,the ref should be 5.0V(Typical)
}

byte ZE07CO_Sensor::checkSum(byte array[],byte length)	
{
    byte sum = 0;
    for(int i = 1; i < length-1; i ++)
    {
		sum += array[i];
    }
    sum = (~sum) + 1;
    return sum;
}

void ZE07CO_Sensor::boucle()
{
  _status = STATUS_WAITING;
  if (mySerial->available()){
    uint8_t ch = mySerial->read();
    switch (_index){
    case 0:
      if (ch != 0xFF){
        return;
      }
      receivedCommandStack[_index]=ch;
      break;
    case 1:
      if (ch != 0x04){
        _index = 0;
        return;
      }
      receivedCommandStack[_index]=ch;
      break;
    case 2:
     if (ch != 0x03){
        _index = 0;
        return;
      }
      receivedCommandStack[_index]=ch;
      break;
    default:
      if (_index==8){
        _status = STATUS_OK;
      
      }
      receivedCommandStack[_index]=ch;
      break;
    }
    _index++;
  }
  
}

boolean ZE07CO_Sensor::available(uint16_t timeout)		//new data was recevied
{
    if (timeout > 0){
		uint32_t start = millis();
		do{
			boucle();
			if (_status == STATUS_OK) break;
		} while (millis() - start < timeout);
	} else {
    boucle();
	}
	if ((receivedCommandStack[MAXLENGTH-1]==checkSum(receivedCommandStack,MAXLENGTH)) && (_status = STATUS_OK)){
		receivedFlag = 1;			//new data received
		return receivedFlag;
	} else {
		receivedFlag = 0;			//data loss or error
		return receivedFlag;
	}
}

float ZE07CO_Sensor::uartReadPPM()
{
    receivedFlag = 0;
    float ppm = (unsigned int)receivedCommandStack[4]<<8 | receivedCommandStack[5];		// bit 4: ppm high 8-bit; bit 5: ppm low 8-bit
    ppm = ppm / 10.0;
    return ppm;
}
 
float ZE07CO_Sensor::dacReadPPM()
{
   float analogVoltage = analogRead(_sensorPin) / 1024.0 * _ref;
   float ppm = (3.125 * analogVoltage - 1.25) * 100;	//linear relationship(0.4V for 0 ppm and 2V for 500ppm)
   if(ppm<0)
	   ppm=0;
   else if(ppm>500)
	   ppm = 500;
   return ppm;
}