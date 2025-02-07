#include <VectorStats.h>

VectorStats::VectorStats(int max_buffer_size)
    : _data_array(max_buffer_size), _max_buffer_size(max_buffer_size), _size(max_buffer_size), _odd_parity(max_buffer_size % 2) {
        _data_array.reserve(max_buffer_size);
    }


// Resizes buffer only if new size is not greater than max_buffer_size.
// Max size of vector must be preallocated on microcontrollers.
void VectorStats::resize(int buffer_size) {
    if (buffer_size <= _data_array.capacity()) {
        _size = buffer_size;
        _odd_parity = buffer_size % 2;
        _data_array.resize(buffer_size);
        zeroBuffer();
    }
}

void VectorStats::add(int16_t value) {
    _data_array[_element] = value;
    if (_element < _size - 1) {
        _element++;
        _buffer_full = false;
        _data_sorted = false;
    } else {
        _element = 0;
        _data_ordered = true;
        _data_sorted = false;
        _buffer_full = true;
    }
}

void VectorStats::fillBuffer(int16_t value) {
    fill(_data_array.begin(), _data_array.end(), value);
    _data_ordered = true;
    _data_sorted = false;
    _buffer_full = true;
}

// Uses the nth_element algorithm to limit cost of sorting all data.
int16_t VectorStats::getMedian() {
    int16_t median;

    if (!_data_sorted) {
        if (_odd_parity) {
            nth_element(_data_array.begin(), _data_array.begin() + _size / 2, _data_array.end());
        } else {
            nth_element(_data_array.begin(), _data_array.begin() + _size / 2 - 1, _data_array.end());
        }
    }
    
    if (_odd_parity) {
        median = _data_array[_size / 2];
        } else {
        median = (_data_array[_size / 2 - 1] + _data_array[_size / 2]) / 2.0 + 0.5;  // +0.5 used to round to int
    }

    _data_ordered = false;
    _buffer_full = false;
    return median;
}

// Can hold up to 6-7 sig figs.
float VectorStats::getAverage() {
    float sum = accumulate(_data_array.begin(), _data_array.end(), 0.0);
    _buffer_full = false;
    return sum / _size;
}

// Gets population standard deviation.
// Uses 4 byte floats yielding 6-7 sig figs.
float VectorStats::getStdDev() {
    _buffer_full = false;

    float sum = accumulate(_data_array.begin(), _data_array.end(), 0.0);
    float mean = sum / _size;
    float variance = inner_product(_data_array.begin(), _data_array.end(), _data_array.begin(), 0.0,
        std::plus<float>(), [mean](float x, float y) { return (x - mean) * (y - mean); });
    return sqrt(variance / _size);
}

// Returns -1 for all values if called after getSortedElement() or getMedian()
int16_t VectorStats::getElement(int element) const {
    if (!_data_ordered) return -1;
    else return _data_array[element];
}

int16_t VectorStats::getSortedElement(int element) {
    if (!_data_sorted) {
        sort(_data_array.begin(), _data_array.end());
        _data_sorted = true;
        _data_ordered = false;
    }
    return _data_array[element];
}

bool VectorStats::isBufferFull() const {
    return _buffer_full;
}

void VectorStats::setBufferFullFalse() {
    _element = 0;
    _buffer_full = false;
    _data_sorted = false;
    _data_ordered = false;
}

void VectorStats::zeroBuffer() {
    fill(_data_array.begin(), _data_array.end(), 0);
    _element = 0;
    _buffer_full = false;
    _data_sorted = false;
    _data_ordered = true;
}

void VectorStats::compareFirstLast() const {
    int avg_last3 = 0;
    
    for (int i = _size - 3; i < _size; ++i) {
        avg_last3 += _data_array[i];
    }
    avg_last3 = avg_last3 / 3.0 + 0.5;
    Serial.print(avg_last3);
    Serial.print(" --> ");
    for (int i = 0; i < _size && _data_array[i] < avg_last3; ++i) {
        Serial.print(_data_array[i]);
        Serial.print(" ");
    }
    Serial.println();
}

std::vector<int16_t> _data_array;
int _max_buffer_size;
int _size;
int _element = 0;
bool _odd_parity;
bool _buffer_full = false;
bool _data_sorted = false;  // Is data sorted smallest to largest.
bool _data_ordered = true;  // Is data in original order for getElement()
