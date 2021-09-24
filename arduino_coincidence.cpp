#include "Arduino.h"
#include "libs/counter.h"

/*This sketch performs testing of the SN74LV8154 using test signals generated by Arduino
 * on the inputs to Counter A and Counter B.  As configured, the sketch utilizes a parallel-to-serial
 * shift register (SN54HC165) to transfer counts from the storage register in the Counter IC to
 * the Arduino.  Alternatively, the sketch can be configured to read the parallel outputs directly from the
 * Counter IC by defining the parallel input pins and commenting out the portions of the code related to
 * the shift register.
 */

CounterIC counter;

void setup() {
    counter.setup();

    Serial.begin(115200);
}

void loop() {
    if (Serial.available() > 0) {
        uint8_t in_byte = Serial.read();

        if (in_byte == 0x6D) {
            // "m" input gets Counter A and B values and prints to serial terminal
            uint32_t counts = counter.read_counter();
            Serial.println("*******************************");
            Serial.print("Counts: ");
            Serial.println(counts);
            Serial.println("*******************************");
        } else if (in_byte == 0x63) {
            //"c" input clears the counters
            counter.clear_counter();
            Serial.println("Counters cleared!");
        }
    }
}