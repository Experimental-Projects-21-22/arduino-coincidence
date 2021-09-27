#include "Arduino.h"
#include "libs/counter.h"

CounterIC counter;

void setup() {
    // Create a serial connection for receiving commands and outputting data.
    Serial.begin(115200);
    // Set up the counter.
    counter.setup();
}

void loop() {
    // Check if there is a command available.
    if (Serial.available() == 0) return;

    uint8_t in_byte = Serial.read();

    if (in_byte == 0x6D) {
        // "m" input gets Counter A and B values and prints to serial terminal
        uint32_t count = counter.read_counter();
        Serial.println("*******************************");
        Serial.print("Counts: ");
        Serial.println(count);
        Serial.println("*******************************");
    } else if (in_byte == 0x63) {
        // "c" input clears the counters
        counter.reset_counter();
        Serial.println("Counters cleared!");
    }

}