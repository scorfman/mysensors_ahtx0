/**
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2015 Sensnology AB
 * Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 *******************************
 *
 * REVISION HISTORY
 * Version 1.0: scorfman
 * 
 * DESCRIPTION
 * This sketch provides an example of how to implement a humidity/temperature
 * sensor using an AHT10 or AHT20 sensor.
 * 
 */
#define SKETCH_NAME "mysensors_ahtx0"
#define SKETCH_VERSION "1.0"
// Enable debug prints
#define MY_DEBUG

// Enable and select radio type attached 
#define MY_RADIO_RF24
//#define MY_RADIO_RFM69
//#define MY_RS485

#include <MySensors.h>  

#define CHILD_ID_HUM  0
#define CHILD_ID_TEMP 1

static bool metric = false;

// Set this offset if the sensor has a permanent small offset to the real temperatures.
// In Celsius degrees (as measured by the device)
#define SENSOR_TEMP_OFFSET 0

// Sleep time between sensor updates (in milliseconds)
static const uint64_t UPDATE_INTERVAL = 5000;

#include <Adafruit_AHTX0.h>
Adafruit_AHTX0 sensor;

void presentation()  
{ 
  // Send the sketch info to the gateway
  sendSketchInfo("mysensors_ahtx0", "1.0");

  // Present sensors as children to gateway
  present(CHILD_ID_HUM, S_HUM);
  present(CHILD_ID_TEMP, S_TEMP);

  metric = getControllerConfig().isMetric;
}

void setup() {
  if (! sensor.begin()) {
    Serial.println("AHTx0 not found.");
    while (1) delay(10);
  }
  Serial.println("AHTx0 found.");
}

void loop()      
{  
  // Read humidity & temperature from the sensor.
  sensors_event_t humidity, temp;
  // populate humidity & temperature objects with fresh data
  sensor.getEvent(&humidity, &temp);
  // apply the offset before converting to something different than Celsius degrees
  temp.temperature += SENSOR_TEMP_OFFSET;

  // Convert celcius to farenheight if metric is false.      
  if (!metric) {
    temp.temperature = (((temp.temperature * 9)/5)+32);
  }
#ifdef MY_DEBUG
  Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" degrees ");
  Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.println("% rH");
#endif

  static MyMessage msgHum( CHILD_ID_HUM,  V_HUM );
  static MyMessage msgTemp(CHILD_ID_TEMP, V_TEMP);

  send(msgTemp.set(temp.temperature, 2));
  send(msgHum.set(humidity.relative_humidity, 2));

  // Sleep for the update interval
  sleep(UPDATE_INTERVAL); 
}
