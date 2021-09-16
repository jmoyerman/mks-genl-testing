#include <Arduino.h>

#include "NonBlockingSerialDriver.hpp"

String echoData(String data) {
  Serial.println("New Data: " + data);
  return "";
}

NonBlockingSerialDriver* serialDriver;
void setup()
{
  Serial.begin(9600);
  serialDriver = new NonBlockingSerialDriver(&Serial, echoData);
  Serial.println("<Serial is ready>");
}

void loop()
{
  serialDriver->loop();
}
