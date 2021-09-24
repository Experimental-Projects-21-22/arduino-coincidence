#include "Arduino.h"
#include "libs/counter.h"
#include "math.h"

/*This sketch performs testing of the SN74LV8154 using test signals generated by Arduino
 * on the inputs to Counter A and Counter B.  As configured, the sketch utilizes a parallel-to-serial
 * shift register (SN54HC165) to transfer counts from the storage register in the Counter IC to
 * the Arduino.  Alternatively, the sketch can be configured to read the parallel outputs directly from the
 * Counter IC by defining the parallel input pins and commenting out the portions of the code related to
 * the shift register.
 */

CounterIC cnt(CounterIC::Mode::DUAL);

uint32_t tf;
uint32_t t0_1;
uint32_t t0_2;

void setup() {
    cnt.GAL_pin = 24;
    cnt.GAU_pin = 25;

    cnt.GBL_pin = 26;
    cnt.GBU_pin = 27;

    cnt.RCLK_pin = 28;

    cnt.CCLR_pin = 29;

    uint8_t dpins[] = {30, 31, 32, 33, 34, 35, 36, 37};
    cnt.set_data_pins(dpins);

    cnt.CLKA_pin = 22;
    cnt.CLKB_pin = 23;

    cnt.a_freq = 100;
    cnt.b_freq = 10;
    cnt.init();

    Serial.begin(115200);
    tf = millis();
    t0_1 = millis();
    t0_2 = millis();
}

void loop() {
    cnt.update();

    if (Serial.available() > 0) {
        uint8_t in_byte = Serial.read();

        //Spacebar input gets Counter A and B values and prints to serial terminal
        if (in_byte == 0x20) {
            uint32_t countsA = cnt.readCounter(CounterIC::Register::A);
            uint32_t countsB = cnt.readCounter(CounterIC::Register::B);
            Serial.println("*******************************");
            Serial.print("Counts on A: ");
            Serial.println(countsA);
            Serial.print("Counts on B: ");
            Serial.println(countsB);
            Serial.println();
            Serial.print("Elapsed time since last check: ");
            Serial.print((tf - t0_1) / 1000.0, 3);
            Serial.println(" sec");
            Serial.print("Expected Counts on A: ");
            Serial.println((int) floor(cnt.a_freq * (tf - t0_2) / 1000.0));
            Serial.print("Expected Counts on B: ");
            Serial.println((int) floor(cnt.b_freq * (tf - t0_2) / 1000.0));
            Serial.println();
            t0_1 = millis();
        }

            //"c" input clears the counters
        else if (in_byte == 0x63) {
            Serial.println("Counters cleared!");
            cnt.clearCounters();
            t0_2 = millis();
        }

            //"t" input toggles counter B on/off
        else if (in_byte == 0x74) {
            cnt.toggleCounterB();
            if (cnt.enabledCounterB()) {
                Serial.println("Counter B ON");
            } else {
                Serial.println("Counter B OFF");
            }
        }


    }

    tf = millis();

}