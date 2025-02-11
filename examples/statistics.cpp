#include <Arduino.h>
#include "VectorStats.h"

const unsigned long BAUD_RATE = 115200;

// Create a buffer of type <int16_t> with max_buffer_size of 11:
VectorStats<int16_t> test_buffer(11);


// A function to help print sorted buffer elements:
// Note: it is not necessary to sort the buffer to get any statistics.
void printBuffer() {
    Serial.print("\nBuffer elements: ");
    for (int i = 0; i < test_buffer.size(); ++i) {
        Serial.print(test_buffer.getSortedElement(i));
        Serial.print(" ");
    }
    Serial.println();
}

// A function to add random numbers to the buffer:
void randomBuffer() {
    for (int i =0; i < 100; ++i) {
        if (!test_buffer.bufferFull()) {
            test_buffer.add(rand() % 10);
        }
    }
}


void setup() {
    Serial.begin(BAUD_RATE);
    while(!Serial.available()) {
        Serial.println("Press any key to begin...");
        delay(1000);
    }

    // The rest of this code should likely go in the loop() function.
    // For demonstration purposes, it is placed in the setup() function.
    Serial.println("------------ Beginning example ------------");


    // Get Median of Odd Sized Buffer:
    // This is faster than even-sized buffers.
    randomBuffer();
    printBuffer();  // Buffer elements: 0 1 2 2 3 3 6 6 8 9 9 
    Serial.print("Median: ");
    Serial.println(test_buffer.getMedian());  // Median: 3


    // Get Average:
    Serial.print("Average: ");
    Serial.println(test_buffer.getAverage());  // Average: 4.45


    // Get Standard Deviation:
    Serial.print("Standard Deviation: ");
    Serial.println(test_buffer.getStdDev());  // Standard Deviation: 3.11


    // Get Median of Even Sized Buffer:
    // This will be the average of the two middle elements.
    test_buffer.resize(10);
    randomBuffer();
    printBuffer();  // Buffer elements: 0 1 2 3 3 5 6 7 8 9 
    Serial.print("Median: ");
    Serial.println(test_buffer.getMedian());  // Median: 4 --> (3 + 5) / 2 = 4


    // Get Slope of Trending Data:
    /// Note: The data trends downward.
    // Useful to determine if data has stabilized.
    // Retruns the slope of the data set.
    test_buffer.resize(11);
    int sloped_data[11] = {2791, 2790, 2788, 2783, 2775, 2775, 2771, 2760, 2759, 2760, 2755};
    Serial.print("\nSlope Buffer elements: ");
    for (int i : sloped_data) {
        test_buffer.add(i);
        Serial.print(i);
        Serial.print(" ");
    }
    Serial.print("\nSlope: ");
    Serial.println(test_buffer.getSlope());  // Slope: -3.97  --> Negative because data trends downward.


    // Get Outliers:
    // Returns the count of all elements that are n standard deviations from the mean.
    // Default n = 2.
    int outlier_data[11] = {2791, 2082, 2082, 2084, 2084, 1367, 2084, 1377, 2084, 2793, 2083};
    Serial.print("\nOutlier Buffer elements: ");
    for (int i : outlier_data) {
        test_buffer.add(i);
        Serial.print(i);
        Serial.print(" ");
    }

    Serial.print("\nOutliers: ");
    Serial.println(test_buffer.getOutliers(1));  // Outliers: 4  --> 2791, 1367, 1377, 2793


    // Get Left Skew:
    // Returns the count of left elements that are > n standard deviations from the mean.
    // Default n = 2.
    // To be effective, buffer should really be much larger.
    // Most useful when you need to determine how long to pause before taking data.
    // Such as when determining the charge time for a capacitor.
    int pos_skewed_data[11] = {2015, 2005, 2003, 2001, 2003, 2001, 2002, 2000, 2004, 2001, 2002};
    Serial.print("\nSkew Buffer elements: ");
    for (int i : pos_skewed_data) {
        test_buffer.add(i);
        Serial.print(i);
        Serial.print(" ");
    }

    Serial.print("\nLeft Skew(1): ");
    Serial.println(test_buffer.getLeftSkew(1));  // Left Skew: 3  --> 2015, 2005, 2003 
    Serial.print("Left Skew(2): ");
    Serial.println(test_buffer.getLeftSkew());  // Left Skew: 2  --> 2015, 2005 (default n = 2)
    Serial.print("Left Skew(3): ");
    Serial.println(test_buffer.getLeftSkew(3));  // Left Skew: 1  --> 2015
    // Skews are positive due to the higher values on the left side of the buffer.
    // Note: The second 2003 and 2004 values are never captured because good values appear before them.


}



void loop() {

}