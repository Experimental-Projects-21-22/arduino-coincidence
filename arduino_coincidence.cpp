#include "Arduino.h"
#include "libs/counter.h"

CounterIC counter;

uint32_t counts[CounterIC::counters] = {0};

void setup() {
    // Create a serial connection for receiving commands and outputting data.
    Serial.begin(115200);
    // Set up the counter.
    counter.setup();
}

void loop() {
    // Check if there is a command available.
    if (Serial.available() == 0) return;

    // This byte will contain the command to execute.
    uint8_t in_byte = Serial.read();

    if (in_byte == 0x73) {
        // "s" saves the value in the counters to their registers.
        counter.save_counts_to_register();
        Serial.println("Counts saved to register!");
    } else if (in_byte == 0x6D) {
        // "m" reads the stored values and prints them to serial.
        counter.read_counters(counts);

        Serial.println("*******************************");
        for (int i = 0; i < CounterIC::counters; ++i) {
            Serial.print("Counts on counter ");
            Serial.print(i);
            Serial.print(": ");
            Serial.println(counts[i]);
        }
        Serial.println("*******************************");
    } else if (in_byte == 0x63) {
        // "c" input clears the counters
        counter.reset_counter();
        Serial.println("Counters cleared!");
    }

}