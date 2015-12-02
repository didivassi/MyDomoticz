/*
  Filters:
  * NONE (default): No filtering
  * MEDIAN: Take the median of a sample
  * HIGHEST_MODE: Take the mode of a sample. In case more than
  one mode, the highest one is returned
  * LOWEST_MODE: Take the mode of a sample. In case more than
  one mode, the lowest one is returned
  * BEST: Take the mode of a sample. In case more than one
  mode is found, the median is returned
  * SIMPLE: Continue reading until sample_size consecutive readings
  are issued

*/
#include "Maxbotix.h"

Maxbotix rangeSensorPW(8, Maxbotix::PW, Maxbotix::LV, Maxbotix::BEST);
int rangeSensorPower = 13;
int raspberryComm = 7;
bool on = false;

void toggleSensor(bool target_state) {
  digitalWrite(rangeSensorPower, target_state);
  on = target_state;
}

void updateSensorStatus() {
  toggleSensor(digitalRead(raspberryComm));
  delay(250);
}

void setup()
{
  Serial.begin(9600);
  pinMode(rangeSensorPower, OUTPUT);
  pinMode(raspberryComm, INPUT);
  on = false;  
}

void loop()
{
  updateSensorStatus();
  
  if(on) {
    Serial.println(rangeSensorPW.getRange());
  } else {
    Serial.println(0.00);
  }
  
  delay(10);
}
