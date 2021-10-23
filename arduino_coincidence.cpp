#include "Arduino.h"
#include "libs/counter.h"
#include "libs/delay.h"

// The counter interface
CounterIC counter;

// Digital pins used by the delay lines
uint8_t CLK_pin = 3;
uint8_t D_pin = 2;
uint8_t WLE1_pin = 4;
uint8_t CLE1_pin = 5;
uint8_t WLE2_pin = 7;
uint8_t CLE2_pin = 6;

// The various delay lines (use a common serial interface)
SerialInterface serial_interface(CLK_pin, D_pin);
DelayLine delay_WA(WLE1_pin, serial_interface);
DelayLine delay_CA(CLE1_pin, serial_interface);
DelayLine delay_WB(WLE2_pin, serial_interface);
DelayLine delay_CB(CLE2_pin, serial_interface);

// Buffer for reading out the counters
uint32_t counts[CounterIC::counters] = {0};

// Value used to store numerical data sent to the Arduino
uint8_t target_value = 0;

DelayLine get_delay_line(char line, char detector_id) {
    if (line == 'C' and detector_id == 'A') {
        return delay_CA;
    } else if (line == 'C' and detector_id == 'B') {
        return delay_CB;
    } else if (line == 'W' and detector_id == 'A') {
        return delay_WA;
    } else if (line == 'W' and detector_id == 'B') {
        return delay_WB;
    }
}

void setup() {
    // Create a serial connection for receiving commands and outputting data.
    Serial.begin(115200);
    // Set up the counter.
    counter.setup();
    // Set up the delay lines.
    serial_interface.setup();
    delay_WA.setup();
    delay_CA.setup();
    delay_WB.setup();
    delay_CB.setup();
}

void loop() {
    // Take a look at the next character
    int next_byte = Serial.peek();
    if (next_byte == -1) return;

    if ('0' <= next_byte and next_byte <= '9') {
        // Completely read in numerical values
        target_value = (uint8_t) Serial.parseInt();
        // Here we should do something with the received value.
        Serial.print("Received a number: ");
        Serial.println(target_value);
    } else {
        String command = Serial.readStringUntil('\n');

        if (command == "SAVE") {
            // Saves the value in the counters to their registers.
            counter.save_counts_to_register();
            Serial.println("Counts saved to register!");
        } else if (command == "READ") {
            // Reads the stored values and prints them to serial.
            counter.read_counters(counts);

            Serial.println("*******************************");
            for (int i = 0; i < CounterIC::counters; ++i) {
                Serial.print("Counts on counter ");
                Serial.print(i);
                Serial.print(": ");
                Serial.println(counts[i]);
            }
            Serial.println("*******************************");
        } else if (command == "CLEAR") {
            // Clears the counters
            counter.reset_counter();
            Serial.println("Counters cleared!");
        } else if (command.startsWith("SD")) {
            char line = command.charAt(2);
            char detector_id = command.charAt(3);
            get_delay_line(line, detector_id).set_delay(target_value);
        } else if (command.startsWith("ID")) {
            char line = command.charAt(2);
            char detector_id = command.charAt(3);
            get_delay_line(line, detector_id).increment_delay(target_value);
        } else if (command.startsWith("DD")) {
            char line = command.charAt(2);
            char detector_id = command.charAt(3);
            get_delay_line(line, detector_id).decrement_delay(target_value);
        }
    }
}