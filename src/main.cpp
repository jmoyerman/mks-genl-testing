#include <Arduino.h>

#include "NonBlockingSerialDriver.hpp"


String echoData(String data) {
  Serial.println("New Data: " + data);
  return "";
}

NonBlockingSerialDriver* serialDriver;
void setup()
{
  //Serial.begin(9600);
  //Serial.println("<Serial is ready>");

  Serial.begin(9600);
  serialDriver = new NonBlockingSerialDriver(&Serial, echoData);
}

void loop()
{
  serialDriver->loop();
}

void serialEvent() {
  //recvWithStartEndMarkers();
  //showNewData();
}