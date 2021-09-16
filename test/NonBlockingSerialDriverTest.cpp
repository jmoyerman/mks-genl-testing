#include <ArduinoFake.h>
#include <unity.h>

#include <NonBlockingSerialDriver.hpp>

using namespace fakeit;

String lastMessage;

String throwErrorCallback(String input) {
    throw input;
}

String recordLastMessageCallback(String input) {
    lastMessage = input;
    return "";
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

void test_SimpleMessage_OneIteration() {
    // Setup
    Serial_* serial = ArduinoFakeMock(Serial);
    NonBlockingSerialDriver *underTest = new NonBlockingSerialDriver(serial, recordLastMessageCallback);
    When(Method(ArduinoFake(Serial), available)).Return(1, 1, 0);
    When(Method(ArduinoFake(Serial), read)).Return(':', '#');

    // Execute
    underTest->loop();

    // Asserts
    TEST_ASSERT_EQUAL_STRING(":#", lastMessage.c_str());
}

void test_SimpleMessage_TwoIterations() {
    // Setup
    Serial_* serial = ArduinoFakeMock(Serial);
    NonBlockingSerialDriver *underTest = new NonBlockingSerialDriver(serial, recordLastMessageCallback);
    When(Method(ArduinoFake(Serial), available)).Return(1, 0, 1, 0);
    When(Method(ArduinoFake(Serial), read)).Return(':', '#');

    // Execute
    underTest->loop();
    underTest->loop();

    // Asserts
    TEST_ASSERT_EQUAL_STRING(":#", lastMessage.c_str());
}

void test_VerifyAck() {
    // Setup
    Serial_* serial = ArduinoFakeMock(Serial);
    NonBlockingSerialDriver *underTest = new NonBlockingSerialDriver(serial, recordLastMessageCallback);
    When(Method(ArduinoFake(Serial), available)).Return(1, 0);
    When(Method(ArduinoFake(Serial), read)).Return(0x06);
    When(OverloadedMethod(ArduinoFake(Serial), write, size_t(uint8_t))).Return(1);

    // Execute
    underTest->loop();

    // Asserts
    Verify(OverloadedMethod(ArduinoFake(Serial), write, size_t(uint8_t)).Using('1')).Once();
}

void setUp(void) {
    lastMessage = "";
}


int main() {
    UNITY_BEGIN();

    RUN_TEST(test_NoDataAvailable);
    RUN_TEST(test_SimpleMessage_OneIteration);
    RUN_TEST(test_SimpleMessage_TwoIterations);

    RUN_TEST(test_VerifyAck);

    UNITY_END();

    return 0;
}