/*
FarmBOX Wio Code - wio-side code enabling communication between the Wio and Raspberry Pi inside a FarmBOX
Copyright (C) 2022 Michael Reeves

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <Arduino.h>
#include "disk91_LoRaE5.h"
Disk91_LoRaE5 lorae5(&Serial);
#define Frequency DSKLORAE5_ZONE_EU868
/*
In theoretical production, users can choose there desiered band when ordering from the website.
If you are a DIY users, please select your desiered band, and replace it with the DSKLORAE5_ZONE_EU868. Australia = AU915, US = US195, Europe = EU868
Select your frequency band here.
DSKLORAE5_ZONE_EU868
DSKLORAE5_ZONE_US915
DSKLORAE5_ZONE_AS923_1
DSKLORAE5_ZONE_AS923_2
DSKLORAE5_ZONE_AS923_3
DSKLORAE5_ZONE_AS923_4
DSKLORAE5_ZONE_KR920
DSKLORAE5_ZONE_IN865
DSKLORAE5_ZONE_AU915
 */
 
char deveui[] = "2CF7FFFFFFF0A49F";
char appeui[] = "80000FFFFFF00009";
char appkey[] = "2B7E151628FFFFFFFFFF158809CF4F3C";
int setupState = 0;
char buffer[100];
int sensorPin = A0;
int sensorValue = 0;
/* 
In theoretical production, we will generate these values and provide them to users on the User Manual.
If you are a DIY user, please replace these values with the values you noted down in the 'Helium x FarmBOX Quickstart Guide'
*/
void data_decord(int val_1, int val_2, int val_3, int val_4, int val_5, int val_6, int val_7, int val_8, int val_9, uint8_t data[18])
{
  int val[] = {val_1, val_2, val_3, val_4, val_5, val_6, val_7, val_8, val_9};
 
  for(int i = 0, j = 0; i < 8; i++, j += 8)
  {
    if(val[i] < 0)
    {
      val[i] = ~val[i] + 1;
      data[j] = val[i] >> 8 | 0x80;
      data[j+1] = val[i] & 0xFF;
    }
    else
    {
      data[j] = val[i] >> 8 & 0xFF;
      data[j+1] = val[i] & 0xFF;
    }
  }
}
void setup() {
  Serial.begin(115200);
  Serial.flush();
 
  uint32_t start = millis();
  while ( !Serial && (millis() - start) < 1500 );  // Open the Serial Monitor to get started or wait for 1.5"
 
  // init the library, search the LORAE5 over the different WIO port available
  if ( ! lorae5.begin(DSKLORAE5_SEARCH_WIO) ) {
    setupState = 2;
    while(1); 
  }
 
  // Setup the LoRaWan Credentials
  if ( ! lorae5.setup(
        Frequency,     
        deveui,
        appeui,
        appkey
     ) ){
    setupState = 1;
    while(1);         
  }
}
 
void loop() {
  if (setupState == 0) {
      // Try to get the data from serial port
    if (Serial.available() > 0) {    
      int index = 0;
      int numChar = Serial.available();
      if (numChar>50) {
        numChar=50;
      }
      memset(buffer, 0, sizeof(buffer));
      while (numChar--) {
        buffer[index++] = Serial.read();
      }
      // get data from moisture sensor
      sensorValue = analogRead(sensorPin);
      int sensorStatus = 7; //read error
      if (sensorValue > 200) {
        if (sensorValue > 300) {
          if (sensorValue > 400) {
            if (sensorValue > 500) {
              if (sensorValue > 600) {
                if (sensorValue > 700) {
                  sensorStatus = 6; //emergency too wet
                }
                else {
                   sensorStatus = 5; // too wet
                }
              }
              else {
                sensorStatus = 4; // wet
              }
            }
            else {
              sensorStatus = 3; // okay
            }
          }
          else {
            sensorStatus = 2; // dry
          }
        }
        else {
          sensorStatus = 1; // too dry
        }
      } 
      else {
        sensorStatus = 0; // emergency too dry
      }
      uint8_t uploadMe[18];
      data_decord(buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], sensorStatus, uploadMe);
      lorae5.send_sync(              //Sending the sensor values out
        8,                     // LoRaWan Port, FarmBOX does not care about this value, it can be anything you like
        uploadMe,                  // Our FarmBOX Data to send!
        sizeof(uploadMe),          // Size of the FarmBOX data, should be 8 bytes, but just checking
        false,                 // We do not want an acknoledgement
        7,                     // Spread Factor, if you know what you are doing you way modify this value, otherwise 7 is recommended
        14                     // Tx Power in dBm, if you know what you are doing you may modify this value, otherwise 14 is recommended
       ); 
    }
  }
  delay(86400000); //wait 1 day before sending results out again.
}
