#include <ArduinoFake.h>
#include <unity.h>

#include <NonBlockingSerialDriver.hpp>

using namespace fakeit;

const uint8_t NUM_MESSAGES = 2;
String lastMessageArray[NUM_MESSAGES];
uint8_t lastMessageIndex;

String throwErrorCallback(String input) {
    throw input;
}

String recordMessagesCallback(String input) {
    lastMessageArray[lastMessageIndex++] = input;
    return "";
}

void test_NoDataAvailable() {
    // Setup
    Stream* serial = (StreamFakeProxy*)ArduinoFakeMock(Serial);
    NonBlockingSerialDriver *underTest = new NonBlockingSerialDriver(serial, throwErrorCallback);
    When(Method(ArduinoFake(Serial), available)).Return(0);
    
    // Execute
    underTest->loop();
    
    // No exception = pass
}

void test_SimpleMessage_OneIteration() {
    // Setup
    Stream* serial = (StreamFakeProxy*)ArduinoFakeMock(Serial);
    NonBlockingSerialDriver *underTest = new NonBlockingSerialDriver(serial, recordMessagesCallback);
    When(Method(ArduinoFake(Serial), available)).Return(1, 1, 0);
    When(Method(ArduinoFake(Serial), read)).Return(':', '#');

    // Execute
    underTest->loop();

    // Asserts
    TEST_ASSERT_EQUAL_STRING(":#", lastMessageArray[0].c_str());
}

void test_SimpleMessage_TwoIterations() {
    // Setup
    Stream* serial = (StreamFakeProxy*)ArduinoFakeMock(Serial);
    NonBlockingSerialDriver *underTest = new NonBlockingSerialDriver(serial, recordMessagesCallback);
    When(Method(ArduinoFake(Serial), available)).Return(1, 0, 1, 0);
    When(Method(ArduinoFake(Serial), read)).Return(':', '#');

    // Execute
    underTest->loop();
    underTest->loop();

    // Asserts
    TEST_ASSERT_EQUAL_STRING(":#", lastMessageArray[0].c_str());
}

void test_TwoMessages_OneIteration() {
        // Setup
    Stream* serial = (StreamFakeProxy*)ArduinoFakeMock(Serial);
    NonBlockingSerialDriver *underTest = new NonBlockingSerialDriver(serial, recordMessagesCallback);
    When(Method(ArduinoFake(Serial), available)).Return(1, 1, 1, 1, 1, 1, 0);
    When(Method(ArduinoFake(Serial), read)).Return(':', '1', '#',':', '2', '#');

    // Execute
    underTest->loop();

    // Asserts
    TEST_ASSERT_EQUAL_STRING(":1#", lastMessageArray[0].c_str());
    TEST_ASSERT_EQUAL_STRING(":2#", lastMessageArray[1].c_str());
}

void test_VerifyAck() {
    // Setup
    Stream* serial = (StreamFakeProxy*)ArduinoFakeMock(Serial);
    NonBlockingSerialDriver *underTest = new NonBlockingSerialDriver(serial, recordMessagesCallback);
    When(Method(ArduinoFake(Serial), available)).Return(1, 0);
    When(Method(ArduinoFake(Serial), read)).Return(0x06);
    When(OverloadedMethod(ArduinoFake(Serial), write, size_t(uint8_t))).Return(1);

    // Execute
    underTest->loop();

    // Asserts
    Verify(OverloadedMethod(ArduinoFake(Serial), write, size_t(uint8_t)).Using('1')).Once();
}


// TODO really don't like this test, we should have some kind of an error passed up, but atleast we don't just bomb out
//   the processor right now with a buffer overflow
void test_BufferOverflowCase() {
    // Setup
    Stream* serial = (StreamFakeProxy*)ArduinoFakeMock(Serial);
    NonBlockingSerialDriver *underTest = new NonBlockingSerialDriver(serial, recordMessagesCallback);
    When(Method(ArduinoFake(Serial), available)).Return(1_Times(1), 100_Times(1), 1_Times(1),  1_Times(0));
    // (int) cast because arduino is dumb.
    When(Method(ArduinoFake(Serial), read)).Return(1_Times((int)':'), 100_Times((int)'a'), 1_Times((int)'#'));
    When(OverloadedMethod(ArduinoFake(Serial), write, size_t(uint8_t))).Return(1);

    // Execute
    underTest->loop();

    // Asserts
    TEST_ASSERT_EQUAL_STRING(":aaaaaaaaaaaaaaaaaaaaaaaaaaaaa#", lastMessageArray[0].c_str());
}

void setUp(void) {
    lastMessageIndex = 0;
    for (int i = 0; i < NUM_MESSAGES; i++) {
        lastMessageArray[i] = "";
    }
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_NoDataAvailable);
    RUN_TEST(test_SimpleMessage_OneIteration);
    RUN_TEST(test_SimpleMessage_TwoIterations);

    RUN_TEST(test_TwoMessages_OneIteration);
    RUN_TEST(test_VerifyAck);


    RUN_TEST(test_BufferOverflowCase);

    UNITY_END();

    return 0;
}