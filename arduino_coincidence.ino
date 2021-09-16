#include "Arduino.h"
#include "libs/counter.h"
#include "shiftreg.h"
#include "math.h"

/*This sketch performs testing of the SN74LV8154 using test signals generated by Arduino
 * on the inputs to Counter A and Counter B.  As configured, the sketch utilizes a parallel-to-serial
 * shift register (SN54HC165) to transfer counts from the storage register in the Counter IC to
 * the Arduino.  Alternatively, the sketch can be configured to read the parallel outputs directly from the
 * Counter IC by defining the parallel input pins and commenting out the portions of the code related to
 * the shift register.
 */

ShiftRegIC shft;
CounterIC cnt("dual", &shft);
//CounterIC cnt("dual");			//Uncomment for parallel-to-Arduino

uint32_t tf;
uint32_t t0_1;
uint32_t t0_2;

void setup()
{
    cnt.GAL_pin = 48;
    cnt.GAU_pin = 50;
    cnt.GBL_pin = 52;
    cnt.GBU_pin = 53;
    cnt.CCLR_pin = 42;
    cnt.RCLK_pin = 36;
    //uint8_t dpins[] = {16, 17, 18, 19, 20, 21, 22, 23};	//Uncomment for parallel-to-Arduino
    //cnt.set_data_pins(dpins);
    cnt.set_test_pins(30, 31);
    cnt.set_testA_freq(100);
    cnt.set_testB_freq(10);
    cnt.init();


    shft.CLK_pin = 40;
    shft.QH_pin = 22;
    shft.SHLD_pin = 38;
    shft.set_clock_freq(1000);
    shft.init();


    Serial.begin(115200);
    tf = millis();
    t0_1 = millis();
    t0_2 = millis();
}

void loop()
{
    cnt.update();

    if (Serial.available() > 0) {
        uint8_t in_byte = Serial.read();

        //Spacebar input gets Counter A and B values and prints to serial terminal
        if (in_byte == 0x20) {
            uint32_t countsA = cnt.readCounter("A");
            uint32_t countsB = cnt.readCounter("B");
            Serial.println("*******************************");
            Serial.print("Counts on A: ");
            Serial.println(countsA);
            Serial.print("Counts on B: ");
            Serial.println(countsB);
            Serial.println();
            Serial.print("Elapsed time since last check: ");
            Serial.print((tf-t0_1)/1000.0, 3);
            Serial.println(" sec");
            Serial.print("Expected Counts on A: ");
            Serial.println((int) floor(cnt.a_freq*(tf-t0_2)/1000.0));
            Serial.print("Expected Counts on B: ");
            Serial.println((int) floor(cnt.b_freq*(tf-t0_2)/1000.0));
            Serial.println();
            t0_1 = millis();
        }

            //"c" input clears the counters
        else if (in_byte == 0x63) {
            Serial.println("Counters cleared!");
            cnt.clearCounters();
            t0_2 = millis();
        }

            //"i" input gets status of counter A overflow
        else if (in_byte == 0x69) {
            Serial.print("Counter A overflow status: ");
            if (cnt.overFlow()) {
                Serial.print("True");
            }
            else {
                Serial.print("False");
            }
        }

            //"t" input toggles counter B on/off
        else if (in_byte == 0x74) {
            cnt.toggleCounterB();
            if (cnt.enabledCounterB()) {
                Serial.println("Counter B ON");
            }
            else {
                Serial.println("Counter B OFF");
            }
        }


    }

    tf = millis();

}