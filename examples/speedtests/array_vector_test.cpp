/*
 * Using vectors is way faster than arrays:
    * Buffer of size 511: Vector up to 520 times faster.
    * Buffer of size 4095: Vector up to 5000 times faster.
*/

#include <Arduino.h>
#include <vector>
#include <algorithm> 
#include <MillisChronoTimer.h>  // https://github.com/Steve8291/MillisChronoTimer


const int buffer_size = 511;
// const int buffer_size = 4095;

const int THERMISTOR_INPUT_PIN = GPIO_NUM_4;


class My_Array {
    public:
        My_Array(int buffer_size) : _size(buffer_size) {
            _data_array = new int16_t[_size];
        }

    // Destructor to deallocate the array
        ~My_Array() {
            delete[] _data_array;
        }

        void add(int16_t value) {
            _data_array[_element] = value;
            if (_element < _size - 1) {
                _element++;
                _buffer_full = false;
            } else {
                _element = 0;
                _buffer_full = true;
            }
        }

        bool isBufferFull() const {
            return _buffer_full;
        }

        int16_t getMedian() {
            int16_t median;
            std::nth_element(_data_array, _data_array + _size / 2, _data_array + _size);
            median = _data_array[_size / 2];
            _buffer_full = false;  // Reset buffer flag
            return median;
        }

    private:
        int _size;
        int16_t* _data_array;
        int _element = 0;
        bool _buffer_full = false;

};


class My_Vector {
    public:
        My_Vector(int buffer_size) : _size(buffer_size) {
            _data_array.reserve(buffer_size);
        }

        void add(int16_t value) {
            _data_array[_element] = value;
            if (_element < _size - 1) {
                _element++;
                _buffer_full = false;
            } else {
                _element = 0;
                _buffer_full = true;
            }
        }

        bool isBufferFull() const {
            return _buffer_full;
        }

        int16_t getMedian() {
            int16_t median;
            nth_element(_data_array.begin(), _data_array.begin() + _size / 2, _data_array.end());
            median = _data_array[_size / 2];
            _buffer_full = false;  // Reset buffer flag
            return median;
        }

    private:
        int _size;
        std::vector<int16_t> _data_array;
        int _element = 0;
        bool _buffer_full = false;

};

My_Array test_array(buffer_size);
My_Vector test_vector(buffer_size);
MillisChronoTimer data_timer(2000);

void setup() {
    Serial.begin(115200);
    while(!Serial.available()) {
        Serial.println("Press any key to begin...");
        delay(1000);
    }

    pinMode(THERMISTOR_INPUT_PIN, INPUT);
}


void loop() {
    data_timer.reset();
    while (!test_array.isBufferFull()) {
        test_array.add(analogRead(THERMISTOR_INPUT_PIN));
    }
    float arrayBufferTime = data_timer.elapsed();
    data_timer.reset();
    int arrayMedian = test_array.getMedian();
    float arrayMedianTime = data_timer.elapsed();


    data_timer.reset();
    while (!test_vector.isBufferFull()) {
        test_vector.add(analogRead(THERMISTOR_INPUT_PIN));
    }
    data_timer.reset();
    float vectorBufferTime = data_timer.elapsed();
    int vectorMedian = test_vector.getMedian();
    float vectorMedianTime = data_timer.elapsed();


    Serial.print(" Array Times --> ");
    Serial.print("fillBuffer: ");
    Serial.print(arrayBufferTime);
    Serial.print("\tgetMedian: ");
    Serial.print(arrayMedianTime);
    Serial.print("\tmedianValue: ");
    Serial.print(arrayMedian);
    Serial.print("\ttotalTime: ");
    Serial.print(arrayBufferTime + arrayMedianTime);
    Serial.println();

    Serial.print("Vector Times --> ");
    Serial.print("fillBuffer: ");
    Serial.print(vectorBufferTime);
    Serial.print("\tgetMedian: ");
    Serial.print(vectorMedianTime);
    Serial.print("\tmedianValue: ");
    Serial.print(vectorMedian);
    Serial.print("\ttotalTime: ");
    Serial.print(vectorBufferTime + vectorMedianTime);
    Serial.println("\n");

    delay(1000);

}

