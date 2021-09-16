#ifndef _NON_BLOCKING_SERIAL_DRIVER_HPP_
#define _NON_BLOCKING_SERIAL_DRIVER_HPP_

#ifdef UNIT_TEST
    #include <ArduinoFake.h>
#else
    #include <Arduino.h>
#endif

const byte NUM_CHARS = 32;
const char START_CHAR = ':';
const char END_CHAR = '#';
const char ACK_CHAR = 0x06;
const char ACK_REPLY_CHAR = '1';

class NonBlockingSerialDriver
{
  public:
    NonBlockingSerialDriver(Serial_ *serial, String (*callback)(String));

    // maybe? void init(); 

    void loop();
  
  private:
    Serial_ *_serial;
    String (*_callback)(String);
    char _receivedChars[NUM_CHARS];
    boolean _newData;
    uint8_t _i;
};

#endif