#include "NonBlockingSerialDriver.hpp"

NonBlockingSerialDriver::NonBlockingSerialDriver(HardwareSerial *serial, String (*callback)(String)) {
    _serial = serial;
    _callback = callback;
    _newData = false;
    _i = 0;
}

void NonBlockingSerialDriver::loop() {
    // TODO some error case of long running transmissions, or unterminated commands
    char rc;
    while (_serial->available() && _newData == false) {
        rc = _serial->read();

        if (_i == NUM_CHARS - 2) {  // Need gap of 2, one for \0, one for 0 index array
            // TODO error case here
            _i = NUM_CHARS - 2;
        }

        // Process received data
        if (rc == ACK_CHAR) {
            _serial->write(ACK_REPLY_CHAR);
        }
        else if (rc == END_CHAR) {
            _receivedChars[_i] = rc;
            // TODO buffer overflow fix
            _receivedChars[_i+1] = '\0';  // terminate for string length
            _i = 0;
            _callback(_receivedChars);
        }
        else {
            _receivedChars[_i] = rc;
            _i++;
        }
    }
}