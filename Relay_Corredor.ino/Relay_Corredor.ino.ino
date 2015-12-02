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
 * Version 1.0 - Henrik Ekblad
 * 
 * DESCRIPTION
 * Example sketch for a "light switch" where you can control light or something 
 * else from both HA controller and a local physical button 
 * (connected between digital pin 3 and GND).
 * This node also works as a repeader for other nodes
 * http://www.mysensors.org/build/relay
 */ 

// Enable debug prints to serial monitor
#define MY_DEBUG 

// Enable and select radio type attached
#define MY_RADIO_NRF24
//#define MY_RADIO_RFM69

// Enabled repeater feature for this node
#define MY_REPEATER_FEATURE

#include <SPI.h>
#include <MySensor.h>
#include <Bounce2.h>

#define RELAY_PIN  3  // Arduino Digital I/O pin number for relay 
#define BUTTON_PIN  4  // Arduino Digital I/O pin number for button 
#define CHILD_ID 1   // Id of the sensor child
#define RELAY_ON 0
#define RELAY_OFF 1
#define DIGITAL_INPUT_SENSOR 8   // The digital input you attached your motion sensor.  (Only 2 and 3 generates interrupt!)
unsigned long SLEEP_TIME = 120000;

int oldValue=0;
bool state;

MyMessage msg(CHILD_ID,V_STATUS);
MyMessage msgT(CHILD_ID+1, V_TRIPPED);

void setup()  
{  
  // Setup the button
  pinMode(DIGITAL_INPUT_SENSOR,INPUT);

  digitalWrite(RELAY_PIN,HIGH);
  
    // Make sure relays are off when starting up
  digitalWrite(RELAY_PIN, RELAY_OFF);
  // Then set relay pins in output mode
  pinMode(RELAY_PIN, OUTPUT);   
      
  // Set relay to last known state (using eeprom storage) 
  state = loadState(CHILD_ID);
  digitalWrite(RELAY_PIN, state?RELAY_ON:RELAY_OFF);
}

void presentation()  {
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("Relay Corrredor", "1.1");

  // Register all sensors to gw (they will be created as child devices)
  present(CHILD_ID, S_LIGHT, "Luz para o corredor", true);
   present(CHILD_ID+1, S_MOTION,"Detector Movimento Corredor");
}

/*
*  Example on how to asynchronously check for new messages from gw
*/

boolean oldTripped;
void loop() 
{
  
     boolean tripped = digitalRead(DIGITAL_INPUT_SENSOR) == HIGH; 
         
    if (oldTripped != tripped){
     oldTripped = tripped;   
    
     send(msgT.set(tripped?"1":"0"));  // Send tripped value to gw 
     Serial.println(tripped);
    }
} 
 
void receive(const MyMessage &message) {
  // We only expect one type of message from controller. But we better check anyway.
  if (message.isAck()) {
     Serial.println("This is an ack from gateway");
  }

  if (message.type == V_STATUS) {
     // Change relay state
     state = message.getBool();
     digitalWrite(RELAY_PIN, state?RELAY_ON:RELAY_OFF);
     // Store state in eeprom
     saveState(CHILD_ID, state);
    
     // Write some debug info
     Serial.print("Incoming change for sensor:");
     Serial.print(message.sensor);
     Serial.print(", New status: ");
     Serial.println(message.getBool());
   } 
}

