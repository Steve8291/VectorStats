/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This program tests the time it takes for various libraries to add data points and calculate a running average.
// At very large buffer sizes VectorStats is slightly slower when calculating an average.
// With a buffer size of 1024 there is essentially no difference between the libraries below.
// With a buffer size of 4096 VectorStats is on average 1-2 milliseconds slower.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <Arduino.h>
#include <DataTome.h>            // https://github.com/AlexandreHiroyuki/DataTome/tree/main    alexandrehiroyuki/DataTome@^1.8.1
#include <RunningAverage.h>      // https://github.com/RobTillaart/RunningAverage              robtillaart/RunningAverage@^0.4.7
#include <VectorStats.h>         // https://github.com/Steve8291/VectorStats

// const int SAMPLE_SIZE = 512;
// const int SAMPLE_SIZE = 1024;
const int SAMPLE_SIZE = 4096;  // Number of samples to collect.
const int RUNS = 100;          // Number of times to run the test.
const int ANALOG_INPUT = GPIO_NUM_4;
const unsigned long BAUD_RATE = 115200;


// Creates uint32_t array type and also accumulator (sum) type.
// It is possible to have a different data type for array and for sum.
// DataTomeMvAvg<int16_t, int32_t> data_tome(SAMPLE_SIZE);  // array elements int16_t, element sums int32_t.
DataTomeMvAvg<int32_t> data_tome(SAMPLE_SIZE);  // array elements int32_t, element sums int32_t. (Runs fastest at this size)

RunningAverage running_average(SAMPLE_SIZE);  // array is of type float.

VectorStats<int16_t> vector_stats(SAMPLE_SIZE);  // Runs fastest at this size
// VectorStats<int32_t> vector_stats(SAMPLE_SIZE);



void main_program() {
    int counter = 0;
    long int t1, t2;
    float sensorReading;


    // DataTome
    counter = 0;
    t1 = micros();
    while (counter < SAMPLE_SIZE) {
        data_tome.push(analogRead(ANALOG_INPUT));
        counter ++;
    }
    sensorReading = data_tome.get();
    t2 = micros();
    Serial.print(t2 - t1);
    Serial.print("µs   avg: ");
    Serial.print(sensorReading);
    Serial.print("\t\t");


    // RunningAverage
    counter = 0;
    t1 = micros();
    while (counter < SAMPLE_SIZE) {
        running_average.addValue(analogRead(ANALOG_INPUT));
        counter ++;
    }
    sensorReading = running_average.getFastAverage();
    t2 = micros();
    Serial.print(t2 - t1);
    Serial.print("µs   avg: ");
    Serial.print(sensorReading);
    Serial.print("\t\t");


    // VectorStats
    counter = 0;
    t1 = micros();
    while (counter < SAMPLE_SIZE) {
        vector_stats.add(analogRead(ANALOG_INPUT));
        counter++;
    }
    sensorReading = vector_stats.getAverage();
    t2 = micros();
    Serial.print(t2 - t1);
    Serial.print("µs   avg: ");
    Serial.print(sensorReading);
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
    Serial.println("DataTome                   |    RunningAverage             |    VectorStats");

    int run_count = 0;
    while (run_count < RUNS) {
        main_program();
        run_count ++;
    }

    Serial.println("DataTome                   |    RunningAverage             |    VectorStats");

    // Clear the serial buffer.
    while(Serial.available() > 0) {
        char inChar = Serial.read();
    }
    Serial.print("\nPress any key to begin again...");
    while(!Serial.available()) {
        delay(1000);
    }

}


