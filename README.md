# VectorStats
## A fast vector based statistics buffer for analog data collection.
In my tests this library is 1-1.5x faster than other libraries at adding 255 data values and then calculating median.
I originally created this library specifically for use with analog data collection.
It is useful as both a high speed buffer for data collection in a project and also as a way of checking data quality and smoothing. Check out my other program `thermistor_calibration` that uses this library to help determine the best buffer size for oversampling as well as a way to compare taking the average vs the median of your data.

Version 1.0 of this library can only accept `int16_t` data into its buffer. This was done by design as it is the smallest data type that can hold analogRead values from an Arduino or ESP32. The library was designed specifically on an ESP32 but should work on Arduino variants as well.

# Library Usage
Please note that the header file is heavily commented and you can refer to that for more details.
### Creating an instance of the class
```cpp
#include <VectorStats.h>

// This creates a buffer with a max size of 255 elements.
// All elements are initially zeroed.
VectorStats my_buffer = VectorStats(255);
```

### Resize the Buffer
The size can be changed during runtime but not larger than what was passed to the constructor above (255).
Resizing also causes all data to be zeroed.
```cpp
my_buffer.resize(100);
```

### Add Data to the Buffer
Will act as a circular buffer. Every time a value is added it will go into the next higher position in the array.
Adding data to the buffer is designed to be done using the boolean `isBufferFull()` method and a while or if statement.
This allows overwriting of data without the overhead of zeroing it.
```cpp
// Notice the use of the negation `!`

// A blocking while loop:
while (!my_buffer.isBufferFull()) {
    my_buffer.add(analogRead(THERMISTOR_INPUT_PIN));
}

// A non-blocking if statement:
void loop() {
  // Do some stuff
  if (!my_buffer.isBufferFull()) {
      my_buffer.add(analogRead(THERMISTOR_INPUT_PIN));
  }
  // Do some other stuff
}
```

### Fill the Buffer With a Single Value
Every value in the buffer will be set to (2084).
This also sets the isBufferFull() flag to true.
```cpp
my_buffer.fillBuffer(2084);
```

### Get the Median of a Full Buffer
If you primarily plan on using median with your data you can increase speed by setting your buffer size to an odd value.
Even-sized buffers have to average and round the center two numbers.
Will return -1 if buffer is not full.
Retrieving the median will reset `.isBufferFull()` to false.
Also changes the order of values in the buffer. If you also want to access the original elements then do that before calling.
```cpp
  if (my_buffer.isBufferFull()) {
    int16_t median = my_buffer.getMedian();
  } else my_buffer.add(analogRead(THERMISTOR_INPUT_PIN));
```

### Get the Average of a Full Buffer
Always returns the average as a float. Will set `.isBufferFull()` to false.
```cpp
if (my_buffer.isBufferFull()) {
  float average = my_buffer.getAverage();
}
```

### Get the Standard Deviation of a Full Buffer
Calculates the population standard deviation.
Always returns a float. Will set `.isBufferFull()` to false.
```cpp
if (my_buffer.isBufferFull()) {
  float std_dev = my_buffer.getStdDev();
}
```

### Access an Element By Index
This must be done before calling either `getSortedElement()` or `getMedian()` as both of these methods will change the original order of elements in the array. Will return -1 for all values if called on a sorted buffer.
```cpp
int16_t element = my_buffer.getElement(3);
```

### Access Elements in Sorted Order From Smallest to Largest
If you have a buffer size of 100 this would print all the elements to serial.
```cpp
int buffer_size = 100;
for (int i = 0; i < buffer_size; ++i) {
  Serial.println(my_buffer.getSortedElement(i));
}
```

### Set Buffer Full To False
The buffer full flag can be manually set to false. Does not return a value.
```cpp
my_buffer.setBufferFullFalse();
```

### Zero The Buffer
```cpp
my_buffer.zeroBuffer();
```




## Plans for future versions:
- Allowing multiple data types (float, double, and other ints).
- Adding getSlope() function
- Replacing compareFirstLast() with a function that compares the first elements of array agaist stdDev and returns the number of elements that are outside of that. This will be helpful when testing charge times for capacitors and if you generally have a situation where you might need to delay data collection to allow it to smooth out.