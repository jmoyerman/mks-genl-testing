#include <Arduino.h>

#include "NonBlockingSerialDriver.hpp"



char receivedChars[NUM_CHARS];
boolean newData;

void showNewData()
{
  if (newData == true)
  {
    Serial.print("This just in ... ");
    Serial.println(receivedChars);
    newData = false;
  }
}

String echoData(String data) {
  Serial.println("New Data: " + data);
  return "";
}

void recvWithStartEndMarkers()
{
  static boolean recvInProgress = false;
  static byte ndx = 0;

  char rc;

  // if (Serial.available() > 0) {
  while (Serial.available() > 0 && newData == false)
  {
    rc = Serial.read();

    if (recvInProgress == true)
    {
      if (rc != END_CHAR)
      {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= NUM_CHARS)
        {
          // TODO this just overwrites last char in buffer, should probably error
          ndx = NUM_CHARS - 1;
        }
      }
      else
      {
        receivedChars[ndx] = rc;
        // TODO fix buffer overflow issue here.
        receivedChars[ndx + 1] = '\0'; // terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    }

    else if (rc == START_CHAR)
    {
      recvInProgress = true;
      receivedChars[ndx] = rc;
      ndx++;
    }

    else if (rc == ACK_CHAR) {
      Serial.write(ACK_REPLY_CHAR);
    }
  }
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