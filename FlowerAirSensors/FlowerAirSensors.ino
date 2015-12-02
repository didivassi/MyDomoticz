/*
Connection:
 VCC-5v
 GND-GND
 SCL-SCL(analog pin 5)
 SDA-SDA(analog pin 4)
 ADD-NC or GND
 
 DHT pin 2
*/

// Enable debug prints to serial monitor
#define MY_DEBUG 

// Enable and select radio type attached
#define MY_RADIO_NRF24
//#define MY_RADIO_RFM69


#include "DHT.h"
#include <SPI.h>
#include <MySensor.h>  
#include <Wire.h>
#include <BH1750.h>


#define CHILD_ID_LIGHT 1
#define CHILD_ID_TEMP 2
#define CHILD_ID_HUM 3

unsigned long SLEEP_TIME = 3000; // Sleep time between reads (in milliseconds)


DHT dht;
BH1750 lightMeter;
//MySensor gw;

MyMessage msgLight(CHILD_ID_LIGHT, V_LIGHT_LEVEL);
MyMessage msgTemp(CHILD_ID_TEMP, V_TEMP);
MyMessage msgHum(CHILD_ID_HUM, V_HUM);

float lastTemp;
float lastHum;

void setup()
{

  // gw.begin();

  // Send the sketch version information to the gateway and Controller
  //gw.sendSketchInfo("Flower air sensors", "1.0");
  
  // Register all sensors to gateway (they will be created as child devices)
  //gw.present(CHILD_ID_LIGHT, S_LIGHT_LEVEL, "Flower Lix");
  //gw.present(CHILD_ID_TEMP, S_TEMP, "Flower Temp");
 //gw.present(CHILD_ID_HUM, S_HUM, "Flower Hum");
  
  
  //Serial.println();
  
  //Serial.println("Status\tHumidity (%)\tTemperature (C)\tLux (LX)");
  lightMeter.begin();
  dht.setup(3); // data pin 2
}

void presentation(){
  
  sendSketchInfo("Flower air sensors", "1.1");
  present(CHILD_ID_LIGHT, S_LIGHT_LEVEL, "Flower Lux");
  present(CHILD_ID_TEMP, S_TEMP, "Flower Temp");
  present(CHILD_ID_HUM, S_HUM, "Flower Hum");
 
  }
void loop()
{
 delay(dht.getMinimumSamplingPeriod());

  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();

  uint16_t lux = lightMeter.readLightLevel();

  //gw.send(msgLight.set(lux));
  send(msgLight.set(lux));
if (isnan(temperature) or isnan(humidity)) {
      Serial.println("Failed reading temperature or humidity from DHT");
  } else if (temperature != lastTemp or humidity != lastHum) {
    lastTemp = temperature;
    //gw.send(msgTemp.set(temperature, 1));
    send(msgTemp.set(temperature, 1));
    Serial.print("T: ");
    Serial.println(temperature);
    lastHum = humidity;
    //gw.send(msgHum.set(humidity, 1));
    send(msgHum.set(humidity, 1));
    Serial.print("H: ");
    Serial.println(humidity);
  }    

 //gw.sleep(SLEEP_TIME); //sleep a bit
   sleep(SLEEP_TIME); //sleep a bit
  
}

