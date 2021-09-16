#include <ArduinoFake.h>
#include <unity.h>

#include <NonBlockingSerialDriver.hpp>

using namespace fakeit;

String throwErrorCallback(String input) {
    throw 1;
}

void test_NoDataAvailable() {
    // Setup
    Serial_* serial = ArduinoFakeMock(Serial);
    NonBlockingSerialDriver *underTest = new NonBlockingSerialDriver(serial, throwErrorCallback);
    When(Method(ArduinoFake(Serial), available)).Return(0);
    
    // Execute
    underTest->loop();
    
    // No exception = pass
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_NoDataAvailable);

    UNITY_END();

    return 0;
}