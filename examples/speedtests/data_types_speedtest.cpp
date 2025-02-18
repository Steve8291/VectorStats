/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This program tests the time it takes for VectorStats to calculate a median with different data types.
// Tested on an ESP32 that can use a double.
// All data types are very efficent and take nearly the same time.
// There is a very slight μs benefit to using smaller data types.
// The biggest difference is for a double: At 4095 samples it takes about 2ms longer than a float.
// The takeaway message is that you should use the smallest data type that fits your values to save space.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <Arduino.h>
#include <VectorStats.h>         // https://github.com/Steve8291/VectorStats

// const int SAMPLE_SIZE = 511;
// const int SAMPLE_SIZE = 1025;
const int SAMPLE_SIZE = 4095;  // Number of samples to collect.
const int RUNS = 100;          // Number of times to run the test.
const int ANALOG_INPUT = GPIO_NUM_4;
const unsigned long BAUD_RATE = 115200;

VectorStats<int16_t> short_16(SAMPLE_SIZE);  // Runs fastest at this size
VectorStats<int32_t> long_32(SAMPLE_SIZE);
VectorStats<int64_t> long_long_64(SAMPLE_SIZE);
VectorStats<float> float_32(SAMPLE_SIZE);
VectorStats<double> double_64(SAMPLE_SIZE);




void main_program() {
    long int t1, t2;
    int16_t sensorReading;

    // short_16
    t1 = micros();
    while (!short_16.bufferFull()) {
        short_16.add(analogRead(ANALOG_INPUT));
    }
    sensorReading = short_16.getMedian();
    t2 = micros();
    Serial.print(t2 - t1);
    Serial.print("\t\t");

    // long_32
    t1 = micros();
    while (!long_32.bufferFull()) {
        long_32.add(analogRead(ANALOG_INPUT));
    }
    sensorReading = long_32.getMedian();
    t2 = micros();
    Serial.print(t2 - t1);
    Serial.print("\t\t");

    // long_long_64
    t1 = micros();
    while (!long_long_64.bufferFull()) {
        long_long_64.add(analogRead(ANALOG_INPUT));
    }
    sensorReading = long_long_64.getMedian();
    t2 = micros();
    Serial.print(t2 - t1);
    Serial.print("\t\t");

    // float_32
    t1 = micros();
    while (!float_32.bufferFull()) {
        float_32.add(analogRead(ANALOG_INPUT));
    }
    sensorReading = float_32.getMedian();
    t2 = micros();
    Serial.print(t2 - t1);
    Serial.print("\t\t");

    // double_64
    t1 = micros();
    while (!double_64.bufferFull()) {
        double_64.add(analogRead(ANALOG_INPUT));
    }
    sensorReading = double_64.getMedian();
    t2 = micros();
    Serial.print(t2 - t1);
    Serial.print("\t\t");

    Serial.println();
}


void setup() {
    Serial.begin(BAUD_RATE);
    pinMode(ANALOG_INPUT, INPUT);
    while(!Serial.available()) {
        Serial.println("Press any key to begin...");
        delay(1000);
    }

    Serial.println("\nTimes Printed In Microseconds.\n");
}


void loop() {
    Serial.println("short 16   |    long 32    |    long long 64 |  float 32   |    double 64");

    int run_count = 0;
    while (run_count < RUNS) {
        main_program();
        run_count ++;
    }

    Serial.println("short 16   |    long 32    |    long long 64 |  float 32   |    double 64");

    // Clear the serial buffer.
    while(Serial.available() > 0) {
        char inChar = Serial.read();
    }
    Serial.print("\nPress any key to begin again...");
    while(!Serial.available()) {
        delay(1000);
    }

}


