/////////////////////////////////////////////////////////////////////////
// Demonstrates the implementation of a Median.
// Data is added until the buffer is full.
// The median is then calculated and the buffer is filled with new data.
// Accessing .getMedian() is destrutive to the buffer order.
// Do not attempt to use as a circular buffer.
// Uses an int16_t data type but any int or floating point type can be used.
// int16_t is the smallest data type that can hold the range of 0 - 4095.
// Use an odd-sized buffer for faster median calculations.
/////////////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include "VectorStats.h"

const unsigned long BAUD_RATE = 115200;
const int sensor_pin = A0;  // Connect a sensor to pin A0.
unsigned long start_time;

// Create a buffer of type <int16_t> with max_buffer_size of 127:
VectorStats<int16_t> median_buffer(127);



// A function to help print buffer elements:
void printBuffer() {
    Serial.print("\nBuffer elements: ");
    for (int i = 0; i < median_buffer.size(); ++i) {
        Serial.print(median_buffer.getElement(i));
        Serial.print(" ");
    }
    Serial.println();
}

// A function to generate random floats from 0.01 - 5.00:
int16_t randomInt() {
    int16_t random_number = random(0, 4095);
    return random_number;
}


void setup() {
    Serial.begin(BAUD_RATE);
    pinMode(sensor_pin, INPUT);
    while(!Serial.available()) {
        Serial.println("Press any key to begin...");
        delay(1000);
    }

    // Fill buffer with the first reading.
    // median_buffer.fillBuffer(analogRead(sensor_pin));
    median_buffer.fillBuffer(randomInt());  // Sets .bufferFull() to true.
    Serial.println("\n--- Initial Buffer ---");
    start_time = millis();
}



void loop() {
    // Add a new reading to the buffer:
    if (!median_buffer.bufferFull()) {
        // median_buffer.add(analogRead(sensor_pin));
        median_buffer.add(randomInt());
    } else if (millis() - start_time >= 5000) {
        // Print the median of the buffer every 5000 milliseconds (5 seconds):
        printBuffer();
        start_time = millis();
        Serial.print("Median: ");
        Serial.println(median_buffer.getMedian());  // Resets .bufferFull() to false.
    }

}