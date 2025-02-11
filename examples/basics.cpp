#include <Arduino.h>
#include "VectorStats.h"

const unsigned long BAUD_RATE = 115200;

// Create a buffer of type <int> with max_buffer_size of 11:
VectorStats<int> test_buffer(11);


// A function to help print buffer elements:
void printBuffer() {
    Serial.print("Buffer elements: ");
    for (int i = 0; i < test_buffer.size(); ++i) {
        Serial.print(test_buffer.getElement(i));
        Serial.print(" ");
    }
    Serial.println("\n");
}


void setup() {
    Serial.begin(BAUD_RATE);
    while(!Serial.available()) {
        Serial.println("Press any key to begin...");
        delay(1000);
    }


    // The rest of this code should likely go in the loop() function.
    // For demonstration purposes, it is placed in the setup() function.
    Serial.println("------------ Beginning example ------------\n");


    // Add Some numbers to the buffer until it fills:
    for (int i =0; i < 100; ++i) {
        if (!test_buffer.bufferFull()) {
            test_buffer.add(i + 1);
        }
    }
    printBuffer();  // Buffer elements: 1 2 3 4 5 6 7 8 9 10 11 


    // Resize buffer to 5 elements:
    test_buffer.resize(5);
    printBuffer();  // Buffer elements: 0 0 0 0 0 


    // Fill buffer with a single value:
    test_buffer.fillBuffer(8);
    printBuffer();  // Buffer elements: 8 8 8 8 8


    // Attempt to resize buffer greater than max_buffer_size of 11:
    // Buffer will not be resized.
    test_buffer.resize(15);
    printBuffer();  // Buffer elements: 8 8 8 8 8


    // Calling .getElement() on out-of-bounds elements returns -1:
    Serial.print("Element 5: ");
    Serial.println(test_buffer.getElement(5));  // Element 5: -1


    // Print Sorted Elements:
    // Sorting is destructive to the original buffer order.
    test_buffer.resize(11);
    for (int i =0; i < 100; ++i) {
        if (!test_buffer.bufferFull()) {
            test_buffer.add(rand() % 100);  // Fill buffer with random numbers.
        }
    }
    Serial.println();
    printBuffer();

    Serial.print("Sorted Buffer: ");  // Sorted Buffer: 1 2 3 4 5 6 7 8 9 10 11 
    for (int i = 0; i < test_buffer.size(); ++i) {
        Serial.print(test_buffer.getSortedElement(i));
        Serial.print(" ");
    }
    Serial.println("\n");


    // Calling .getElement() after using .getSortedElements will return -1:
    Serial.print("Element 2: ");
    Serial.println(test_buffer.getElement(5));  // Element 2: -1

}



void loop() {

}