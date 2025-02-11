# VectorStats
## A fast vector based statistics buffer for analog data collection.
I originally created this library specifically for use with analog data collection.
It is useful as both a high speed buffer for data collection in a project and also as a way of checking data quality and smoothing. Check out my other program `thermistor_cal` that uses this library to help determine the optimal buffer size for oversampling as well as a way to compare taking the average vs. the median of your data. In tests it does seem to give a very modest speed increase in calculating median for small data sets. With data sets larger than about 500 members the speed increase becomes more noticable. There is no limit to the size of buffer you create, only the memory available on your device.

Version 2.0 of this library can now accept pretty much any integer or floating-point data type that your microcontroller can handle. Using `int16_t` for analogRead values will provide the best performance as it is the smallest data type that can hold analogRead values from an Arduino or ESP32. The library was designed specifically on an ESP32 but should work on Arduino variants as well.

# Library Usage
Please note that the header file is heavily commented and you can refer to that for more details.
### Creating an instance of the class
This creates an <int16_t> buffer with a max size of (255) elements.
If you plan on using `.getMedian()` always try to use odd-sized buffers.
All elements are initially zeroed.
```cpp
#include <VectorStats.h>

VectorStats<int16_t> my_buffer(255);
```

### Get Current Buffer Size
Returns the current size of the buffer. This method simply returns the value of a variable and does incur any additional computation cost. It is useful for looping through data when printing, etc.
```cpp
my_buffer.size();
```

### Resize the Buffer
The size can be changed during runtime but can not be larger than what was passed to the constructor above (255).
Resizing causes all data to be zeroed and sets `.bufferFull()` flag to false unless value entered is greater than initialized size.
```cpp
my_buffer.resize(100);
```

### Zero The Buffer
Zeroes the buffer and sets `.bufferFull()` to false.
```cpp
my_buffer.zeroBuffer();
```

### Add Data to the Buffer
Will act as a circular buffer. Every time a value is added it will go into the next higher position in the array until it is full and then wrap back around to the first position.
For median calculations, adding data to the buffer is designed to be done using the boolean `.bufferFull()` method and a while or if statement.
For average calculations the buffer can be used in a circular fashion by ignoring the `.bufferFull()` flag.
```cpp
// Notice the use of the negation `!`

// A blocking while loop for median calculations:
while (!my_buffer.bufferFull()) {
    my_buffer.add(analogRead(SENSOR_INPUT_PIN));
}
```
```cpp
// A non-blocking if statement for median calculations:
void loop() {
  int16_t sensor_reading;

  // Do some stuff
  if (!my_buffer.bufferFull()) {
      my_buffer.add(analogRead(SENSOR_INPUT_PIN));
  } else {
    sensor_reading = my_buffer.getMedian();  // Accessing median sets .bufferFull() to false.
    // Do something with sensor_reading.
  }
  // Do some other stuff
}
```
```cpp
// Data can just be continually added to buffer when only doing average calculations:
// The average can be retrieved at any time without destroying data order.
// If you plan to access the data before buffer fills you may want to .fillBuffer()
void setup() {
  my_buffer.fillBuffer(analogRead(SENSOR_INPUT_PIN));
}

void loop() {
  my_buffer.add(analogRead(SENSOR_INPUT_PIN));
  // At any time you can assess:
  float avg = my_buffer.getAverage();
}
```

### Fill the Buffer With a Single Value
Every value in the buffer will be set to (2084).
This also sets the `.bufferFull()` flag to true.
```cpp
my_buffer.fillBuffer(2084);
```

### Get the Median of a Full Buffer
If you primarily plan on using median with your data you can increase speed by setting your buffer size to an odd value.
Even-sized buffers have to average and round the center two numbers.
Will return -1 if buffer is not full.
Retrieving the median will reset `.bufferFull()` to false.
Also changes the order of values in the buffer. If you also want to access `.getElement()` then do that before calling.
```cpp
  if (my_buffer.bufferFull()) {
    int16_t median = my_buffer.getMedian();
  } else my_buffer.add(analogRead(SENSOR_INPUT_PIN));
```

### Get the Average of a Full Buffer
Always returns the average as a float. Will not change `.bufferFull()`. Note: you could just ignore the `.bufferFull()` flag and access average whenever as a circular buffer.
```cpp
if (my_buffer.bufferFull()) {
  float average = my_buffer.getAverage();
  my_buffer.setBufferFullFalse();
} else my_buffer.add(analogRead(SENSOR_INPUT_PIN));
```
```cpp
// Waiting until the first full buffer before accessing average:
my_buffer.add(analogRead(SENSOR_INPUT_PIN));
if (mybuffer.bufferFull()) {
  float average = my_buffer.getAverage();
}
```

### Get the Standard Deviation
Calculates the population standard deviation.
Always returns a float.
```cpp
float std_dev = my_buffer.getStdDev();
```

### Access an Element By Index
This must be done before calling either `.getSortedElement()` or `.getMedian()` as both of these methods will change the original order of elements in the array. Will return -1 for all values if called on a sorted buffer. Also returns -1 if element is out of range. Make sure your data type is the same as your buffer type.
```cpp
int16_t element = my_buffer.getElement(3);
```
```cpp
// Printing the entire unaltered buffer:
void printBuffer() {
    Serial.print("Buffer elements: ");
    for (int i = 0; i < my_buffer.size(); ++i) {
        Serial.print(my_buffer.getElement(i));
        Serial.print(" ");
    }
}
```

### Access Elements in Sorted Order From Smallest to Largest
This method is destructive on the original order of the data set. Be sure to completely refill your set before each call. Returns -1 if element is out of range. Make sure your data type is the same as your buffer type.
```cpp
int16_t element = my_buffer.getSortedElement(5);
```
```cpp
// Printing the entire sorted buffer:
void printSortBuffer() {
    Serial.print("Buffer elements sorted: ");
    for (int i = 0; i < my_buffer.size(); ++i) {
        Serial.print(my_buffer.getSortedElement(i));
        Serial.print(" ");
    }
}
```

### Check if Buffer is Full
Calling `.fillBuffer(n)` will set to true. Calling `.getMedian()` will set to false.
```cpp
if (bufferFull()) {
  //Do something
  }
```

### Set Buffer Full To False
The `.bufferFull()` flag can be manually set to false.
```cpp
my_buffer.setBufferFullFalse();
```

### Get Number of Outliers
Returns the number of outliers in the buffer that are n standard deviations from the mean.
See example in `statistics.cpp`.
Default n = 2.
```cpp
int outliers = my_buffer.getOutliers();   // 2 std deviations from mean
int outliers = my_buffer.getOutliers(3);  // 3 std deviations from mean
```

### Get Left Skew Outliers
This is kind of a strange algorithm and is meant to be used to help determine data smoothness before taking readings.
It is usefull any other time initial data readings may be far outside the expected range.
Especially when dealing with capacitors that will cause incorrect readings until they are fully charged.

Returns only the number of beginning (left-side) outliers. The standard deviation and average of the right half of the buffer is used to determine the skew of the left half of the buffer. Elements from the left side are added to the count if they are greater than (n deviations * right stdDev) from the right side average. A negative skew count indicates that the left side values are lower than the right side average. Default n = 2. Raising n will ignore greater deviations and cause the skew count to decrease. Returns -1 if called on a sorted buffer. See the example file `statistics.cpp` for details.
```cpp
int skew = my_buffer.getLeftSkew();   // skew to 2 std deviations
int skew = my_buffer.getLeftSkew(3);  // skew to 3 std deviations
```

### Get Slope
Calculates the slope of that data using linear regression.
Returns the slope as a float. Negative slopes indicate a downward line.
Returns -1 if called on a sorted buffer. See `statistics.cpp` for example.

Works by assigning x and y coordinates as follows:
- x values are a sequence from 1 to buffer size.
- y values are unaltered data set.
```cpp
float slope = my_buffer.getSlope();
```
