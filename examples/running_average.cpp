/////////////////////////////////////////////////////////////////////////
// Demonstrates the implementation of a Running Average.
// Data is added in a circular buffer fashion.
// Averages can be calculated at any time.
// Uses a float data type but any int or floating point type can be used.
// The buffer is filled with random floats or analogRead values.
/////////////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include "VectorStats.h"

const unsigned long BAUD_RATE = 115200;
const int sensor_pin = A0;  // Connect a sensor to pin A0.
unsigned long start_time;

// Create a buffer of type <float> with max_buffer_size of 100:
VectorStats<float> float_buffer(100);



// A function to help print buffer elements:
void printBuffer() {
    Serial.print("\nBuffer elements: ");
    for (int i = 0; i < float_buffer.size(); ++i) {
        Serial.print(float_buffer.getElement(i));
        Serial.print(" ");
    }
    Serial.println();
}

// A function to generate random floats from 0.01 - 5.00:
float randomFloat() {
    float random_number = random(1, 500) / 100.0;
    return random_number;
}

// Function to create float values from analogRead values:
float getVoltage() {
    float voltage = analogRead(sensor_pin) * (5.0 / 1023.0);
    return voltage;
}


void setup() {
    Serial.begin(BAUD_RATE);
    pinMode(sensor_pin, INPUT);
    while(!Serial.available()) {
        Serial.println("Press any key to begin...");
        delay(1000);
    }

    // Fill buffer with the first voltage reading.
    // float_buffer.fillBuffer(getVoltage());
    float_buffer.fillBuffer(randomFloat());
    Serial.println("\n--- Initial Buffer ---");
    printBuffer();
    start_time = millis();
}



void loop() {
    // Add a new voltage reading to the buffer:
    // float_buffer.add(getVoltage());
    float_buffer.add(randomFloat());


    // Print the average of the buffer every 5000 milliseconds (5 seconds):
    if (millis() - start_time >= 5000) {
        printBuffer();
        start_time = millis();
        Serial.print("Average: ");
        Serial.println(float_buffer.getAverage());
    }

}