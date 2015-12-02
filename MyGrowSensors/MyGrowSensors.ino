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
 * Version 1.0 - Henrik EKblad
 * 
 * DESCRIPTION
 * This sketch provides an example how to implement a humidity/temperature
 * sensor using DHT11/DHT-22 
 * http://www.mysensors.org/build/humidity
 */
 


//Water Temp
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 4 on the Arduino
#define ONE_WIRE_BUS 4

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

float waterTemp_float;

//EC Stuff
#include <SoftwareSerial.h>      //we have to include the SoftwareSerial library, or else we can't use it.  
#define rxEC 2                     //define what pin rx is going to be.
#define txEC 3                     //define what pin Tx is going to be.

SoftwareSerial myserial(rxEC, txEC); //define how the soft serial port is going to work. 

char EC_data[48];                  //we make a 48 byte character array to hold incoming data from the EC. 
char computerdata[20];             //we make a 20 byte character array to hold incoming data from a pc/mac/other. 
byte received_from_sensor=0;       //we need to know how many characters have been received.
byte string_received=0;            //used to identify when we have received a string from the EC circuit.

float EC=0;                  //used to hold a floating point number that is the EC.



//Sonar Stuff

#include <NewPing.h>

#define TRIGGER_PIN  5  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     6  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.




//My sensor
// Enable debug prints to serial monitor
#define MY_DEBUG 

// Enable and select radio type attached
#define MY_RADIO_NRF24


#define CHILD_ID_DIST 1
#define CHILD_ID_WATER_TEMP 2
#define CHILD_ID_EC 3
unsigned long SLEEP_TIME = 30; // Sleep time between reads (in milliseconds)
#include <SPI.h>
#include <MySensor.h>  
  

//MySensor gw;
 

MyMessage msgDistance(CHILD_ID_DIST, V_DISTANCE);
MyMessage msgEC(CHILD_ID_EC, V_IMPEDANCE);
MyMessage msgWaterTemp(CHILD_ID_WATER_TEMP, V_TEMP);


void setup()  
{ 
  //gw.begin();

  //Water temp
  sensors.begin(); 
  
  //EC stuff
  myserial.begin(115200);        //enable the software serial port     
  myserial.print("C,0\r"); //disables continuous mode
  myserial.print("L,0\r"); //disables leds
  myserial.print("RESPONSE,0\r"); // disables response from sensors like OK
  //myserial.print("SERIAL,115200\r");
  myserial.print("O,EC,1\r");
  myserial.print("O,TDS,0\r");
  myserial.print("O,S,0\r");
  myserial.print("O,SG,0\r");
     
      /* Calibration 
     *  
      myserial.print("K,1\r");
      Serial.println("Retira da agua");  
     delay(4000);      
     myserial.print("Cal,dry\r");
       delay(2000);    
      Serial.println("Coloca na agua");
      delay(4000);      
      myserial.print("Cal,one,270\r");
       Serial.println("Pronto");   */ 

  // Send the Sketch Version Information to the Gateway
  //gw.sendSketchInfo("FlowerStation", "1.0");

  // Register all sensors to gw (they will be created as child devices)
  //gw.present(CHILD_ID_TEMP, S_DISTANCE);
  //gw.present(CHILD_ID_EC, S_CUSTOM);
  //gw.present(CHILD_ID_WATER_TEMP, S_TEMP);
  
 // metric = gw.getConfig().isMetric;

}

void presentation(){
    // Send the Sketch Version Information to the Gateway
  sendSketchInfo("FlowerStation", "2.0");

  // Register all sensors to gw (they will be created as child devices)
  present(CHILD_ID_DIST, S_DISTANCE, "Water Level");
  present(CHILD_ID_EC, S_MULTIMETER,"EC Meter ");
  present(CHILD_ID_WATER_TEMP, S_TEMP, "Water temp");
  
  }


int oldDistance=0;
int oldEC=0;
int deltaEC=0;
int countEC=0;
int oldTemp=0;

void loop()      
{  
  
   /*
 * Get Reading from EC Circuit
 * 
 * TODO Save Last Values, grab the average from 10 sucessefull readings from EC

*/

//Sonar
unsigned int uS = sonar.ping(); // Send ping, get ping time in microseconds (uS).
int Distance=uS / US_ROUNDTRIP_CM;
 
if (oldDistance != Distance && Distance > 0){
  send(msgDistance.set(Distance, 1));
  oldDistance = Distance;
}

////Water temp
   sensors.requestTemperatures(); // Send the command to get temperatures
   waterTemp_float = sensors.getTempCByIndex(0);
    //send to gateway
    
  if (oldTemp != waterTemp_float){
   send(msgWaterTemp.set(waterTemp_float, 1));
   oldTemp = waterTemp_float;
  }

//send info to sensor
   myserial.print("T,");
   myserial.print (waterTemp_float) ;
   myserial.print('\r'); 
//get single reading
   myserial.print("R\r"); 
 
   
  if(myserial.available() > 0){        //if we see that the EC Circuit has sent a character.
     received_from_sensor=myserial.readBytesUntil(13,EC_data,48); //we read the data sent from EC Circuit until we see a <CR>. We also count how many character have been received.  
     EC_data[received_from_sensor]=0;  //we add a 0 to the spot in the array just after the last character we received. This will stop us from transmitting incorrect data that may have been left in the buffer.       EC=  atof(strtok(EC_data, ","));  //convert data to Float
     EC=  atof(strtok(EC_data, ","));  //convert data to Float

     if(EC>0){  
       
       deltaEC = abs(oldEC-EC);
       
       if(deltaEC> oldEC * 0.1 && oldEC>0){ //hum... seems an error reading if difference > 10%
        countEC ++;
        EC = oldEC;
       }
        
       if(countEC>10){ //must be a good value
           countEC=0;
           EC=  atof(strtok(EC_data, ","));
        } 
        
       if(oldEC != EC){ //if is new reading and values are ok, send them
          send(msgEC.set(EC, 1));
          oldEC=EC;
       }    
     }
        Serial.print("EC: ");
        Serial.println(EC);
        
       
        Serial.print("WT: ");  
        Serial.println(waterTemp_float);
      //}    
       Serial.print("Ping: ");
       Serial.print(Distance); // Convert ping time to distance in cm and print result (0 = outside set distance range)
       Serial.println(" cm");
     }
       



  sleep(SLEEP_TIME); //sleep a bit
}


