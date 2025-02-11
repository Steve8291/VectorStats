//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This program tests the time it takes for various libraries to add data points and calculate the median.
// At larger sample sizes, VectorStats is significantly faster.
// It also outperforms when using even-sized buffers.
// The output of 100 runs below was summed for each library and then the sum of VectorStats was subtracted.
// VectorStats was up to 1.5x faster with a large buffer size of 4095.
// With an even buffer size of 4096, VectorStats was up to 2.5x faster.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <Arduino.h>
#include <DataTome.h>            // https://github.com/AlexandreHiroyuki/DataTome/tree/main    robtillaart/RunningMedian@^0.3.9
#include <RunningMedian.h>       // https://github.com/RobTillaart/RunningMedian/tree/master   alexandrehiroyuki/DataTome@^1.8.1
#include <VectorStats.h>         // https://github.com/Steve8291/VectorStats

const int SAMPLE_SIZE = 4095;  // Number of samples to collect.
const int RUNS = 100;          // Number of times to run the test.
const int ANALOG_INPUT = GPIO_NUM_4;
const unsigned long BAUD_RATE = 115200;


DataTomeAnalysis<uint16_t> data_tome(SAMPLE_SIZE);
RunningMedian running_median = RunningMedian(255);  // Returns float. Max size is 255 elements.
VectorStats<int16_t> vector_stats(SAMPLE_SIZE);


void main_program() {
    int counter = 0;
    long int t1, t2;        // Timer variables.
    int16_t sensorReading;  // 0 - 4095  uint16_t can hold.


    // DataTome
    counter = 0;
    t1 = micros();
    while (counter < SAMPLE_SIZE) {
        data_tome.push(analogRead(ANALOG_INPUT));
        counter ++;
    }
    sensorReading = data_tome.median();
    t2 = micros();
    Serial.print(t2 - t1);
    Serial.print("\t\t");


    // RunningMedian
    counter = 0;
    t1 = micros();
    while (counter < SAMPLE_SIZE) {
        running_median.add(analogRead(ANALOG_INPUT));
        counter ++;
    }
    sensorReading = running_median.getMedian();
    t2 = micros();
    Serial.print(t2 - t1);
    Serial.print("\t\t");


    // VectorStats
    t1 = micros();
    while (!vector_stats.bufferFull()) {
        vector_stats.add(analogRead(ANALOG_INPUT));
    }
    sensorReading = vector_stats.getMedian();
    t2 = micros();
    Serial.print(t2 - t1);
    Serial.print("\t\t");

    Serial.println();
}


void loop() {
    // Clear the serial buffer.
    while(Serial.available() > 0) {
        char inChar = Serial.read();
    }

    Serial.println("DataTome  |  RunningMedian  |  VectorStats");

    int run_count = 0;
    while (run_count < RUNS) {
        main_program();
        run_count ++;
    }

    Serial.println("DataTome  |  RunningMedian  |  VectorStats");

    // Press any key to begin again...
    while(!Serial.available()) {
        delay(1000);
    }

}


void setup() {
    Serial.begin(BAUD_RATE);
    pinMode(ANALOG_INPUT, INPUT);
    while(!Serial.available()) {
        Serial.println("Press any key to begin...");
        delay(1000);
    }

    running_median.setSearchMode(1);  // Sets faster binary serach mode.
    Serial.println("\nTimes Printed In Microseconds.\n");
}