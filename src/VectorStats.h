/**
 * @file VectorStats.h
 * @brief This header file contains declarations for the VectorStats class.
 * @author Steve Hambling
 * @date 2025-02-04
 */

#ifndef VECTORSTATS_H
#define VECTORSTATS_H

#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>

/**
 * @class VectorStats
 * @brief Class to create C++ vector buffers for fast median, average, and standard deviation.
 * @tparam T The data type of the vector and buffer elements.
 */
template <typename T>
class VectorStats {
public:
    /**
     * @brief Constructor for VectorStats.
     * @param max_buffer_size An integer value to initialize the maximum buffer size.
     */
    VectorStats(int max_buffer_size);

    /**
     * @brief Returns current size of buffer.
     * @return Current size of buffer as an integer.
     */
    int size() const;

    /**
     * @brief Resizes and zeroes buffer.
     * @param buffer_size An integer value for new buffer size.
     * - Buffer size must be less than or equal to max_buffer_size.
     * - Entering a buffer size greater than max_buffer_size will have no effect.
     * - Sets bufferFull() to false.
     */
    void resize(int buffer_size);

    /**
     * @brief Zeroes the buffer and sets .bufferFull() to false.
     */
    void zeroBuffer();

    // Add value to array
    /**
     * @brief Adds value to buffer replacing oldest data first if buffer is unsorted.
     * @param value A value of the <initalized data type> to be added to buffer.
     */
    void add(T value);

    // Fill buffer with value.
    /**
     * @brief Fills entire buffer with value.
     * @param value A value of <initalized data type> to fill buffer.
     * Sets .bufferFull() to true.
     */
    void fillBuffer(T value);

    /**
     * @brief Calculates median of buffer data set.
     * - Changes the order of values in buffer.
     * - Sets .bufferFull() to false.
     * @return Median as <initalized data type>
     * - Odd-sized buffers are faster.
     * - Returns rounded average of center two numbers for even-sized buffers.
     */
    T getMedian();

    /**
     * @brief Calculates the average of buffer data set.
     * @return Average as a float.
     */
    float getAverage() const;

    /**
     * @brief Calculates the population standard deviation of buffer data set.
     * @return Standard Deviation as a float.
     */
    float getStdDev() const;

    /**
     * @brief Gets element from unsorted buffer.
     * @param element An integer representing the index value.
     * @return Element as <initalized data type>
     * - Returns -1 for all values if called on a sorted buffer.
     * - Returns -1 if element is out of range.
     * - Do not use after calling getSortedElement() or getMedian().
     */
    T getElement(int element) const;

    /**
     * @brief Gets element from unsorted buffer.
     * @param element An integer representing the index value.
     * @return Element as <initalized data type>
     * - Returns -1 if element is out of range.
     * Changes the order of values in buffer.
     */
    T getSortedElement(int element);

    /**
     * @brief Checks state of buffer.
     * @return Boolean true if buffer is full.
     */
    bool bufferFull() const;

    /**
     * @brief Toggles .bufferFull() flag to false.
     */
    void setBufferFullFalse();

    /**
     * @brief Counts the number of outliers in the buffer.
     * @param deviations An integer value of standard deviations from mean. Default = 2.
     * @return Outlier count as an integer.
     */
    int getOutliers(int8_t deviations = 2) const;

    /**
     * @brief Counts the number of beginning elements that are outliers.
     * - The standard deviation and average of the right half of the buffer is used
     * - to determine the skew of the left half of the buffer.
     * - Counts left elements that are > (n deviations * right stdDev) from the right mean.
     * - A negative skew count indicates the left side is lower than the right side average.
     * @param deviations An integer value of standard deviations from mean. Default = 2.
     * @return Skew count of left elements deviating from right mean as an integer.
     * - Returns -1 if called on a sorted buffer.
     */
    int getLeftSkew(int8_t deviations = 2) const;

    /**
     * @brief Calculates slope using linear regression.
     * @return Slope as a float. Can be negative.
     * - Returns -1 if called on a sorted buffer.
     * - x values are sequence from 1 to buffer size.
     * - y values are unaltered data set.
     */
    float getSlope() const;

private:
    std::vector<T> _data_array;
    const int _max_buffer_size;
    int _size;
    int _mid_element;
    bool _odd_parity;
    int _element;
    bool _buffer_full;
    bool _data_sorted;   // Is data sorted smallest to largest?
    bool _data_ordered;  // Is data in original order?

};


////////////////////////////////////////
// VectorStats Class Implementation
////////////////////////////////////////

template <typename T>
VectorStats<T>::VectorStats(int max_buffer_size)
    : _data_array(max_buffer_size),  // Preallocates memory.
      _max_buffer_size(max_buffer_size),
      _size(max_buffer_size),
      _mid_element(max_buffer_size / 2),
      _odd_parity(max_buffer_size % 2),
      _element(0),
      _buffer_full(false),
      _data_sorted(false),
      _data_ordered(true) {}

template <typename T>
int VectorStats<T>::size() const {
    return _size;
}

// Resizes buffer to any size up to max_buffer_size.
// Max size of vector must be preallocated on microcontrollers.
template <typename T>
void VectorStats<T>::resize(int buffer_size) {
    if (buffer_size <= _max_buffer_size) {
        _size = buffer_size;
        _mid_element = buffer_size / 2;
        _odd_parity = buffer_size % 2;
        _data_array.resize(buffer_size);
        zeroBuffer();
    }
}

template <typename T>
void VectorStats<T>::zeroBuffer() {
    std::fill(_data_array.begin(), _data_array.end(), 0);
    _element = 0;
    _buffer_full = false;
    _data_sorted = false;
    _data_ordered = true;
}

// Does not block if buffer is full.
// Will behave circularly if .bufferFull() is ignored.
template <typename T>
void VectorStats<T>::add(T value) {
    _data_array[_element] = value;
    if (_element < _size - 1) {
        _element++;
        _buffer_full = false;
        _data_sorted = false;
    } else {
        _element = 0;
        _buffer_full = true;
        _data_sorted = false;
        _data_ordered = true;
    }
}

template <typename T>
void VectorStats<T>::fillBuffer(T value) {
    std::fill(_data_array.begin(), _data_array.end(), value);
    _data_ordered = true;
    _data_sorted = false;
    _buffer_full = true;
}

// Uses the nth_element algorithm to limit cost of sorting all data.
template <typename T>
T VectorStats<T>::getMedian() {
    T median;
    T right_mid, left_mid;

    if (!_data_sorted) {
        std::nth_element(_data_array.begin(), _data_array.begin() + _mid_element, _data_array.end());
        right_mid = _data_array[_mid_element];
        if (_odd_parity) {
            median = right_mid;
        } else {
            // Handle even parity.
            std::nth_element(_data_array.begin(), _data_array.begin() + _mid_element - 1, _data_array.end());
            left_mid = _data_array[_mid_element - 1];
            if (std::is_integral<T>::value) {
                median = round((right_mid + left_mid) / 2);
            } else {
            median = (right_mid + left_mid);  // Don't round for floating point types
            }
        }
    } else {
        if (_odd_parity) {
            median = _data_array[_mid_element];
        } else if (std::is_integral<T>::value) {
            median = round((_data_array[_mid_element - 1] + _data_array[_mid_element]) / 2);
        } else {
            median = (_data_array[_mid_element - 1] + _data_array[_mid_element] / 2);
        }
    }

    _data_ordered = false;
    _buffer_full = false;
    return median;
}

// Can hold up to 6-7 sig figs.
template <typename T>
float VectorStats<T>::getAverage() const {
    float sum = std::accumulate(_data_array.begin(), _data_array.end(), 0.0);
    return sum / _size;
}

// Gets population standard deviation.
// Uses 4 byte floats yielding 6-7 sig figs.
template <typename T>
float VectorStats<T>::getStdDev() const {
    float sum = std::accumulate(_data_array.begin(), _data_array.end(), 0.0);
    float mean = sum / _size;
    float variance = std::inner_product(_data_array.begin(), _data_array.end(), _data_array.begin(), 0.0,
        std::plus<float>(), [mean](float x, float y) { return (x - mean) * (y - mean); }) / _size;
    return std::sqrt(variance);
}

// Returns -1 for all values if called after getSortedElement() or getMedian()
template <typename T>
T VectorStats<T>::getElement(int element) const {
    if (_data_ordered && element >= 0 && element < _size) {
        return _data_array[element];
    } else { return -1; }
}

template <typename T>
T VectorStats<T>::getSortedElement(int element) {
    if (!_data_sorted) {
        std::sort(_data_array.begin(), _data_array.end());
        _data_sorted = true;
        _data_ordered = false;
    }

    if (element >= 0 && element < _size) {
        return _data_array[element];
    } else { return -1; }
}

template <typename T>
bool VectorStats<T>::bufferFull() const {
    return _buffer_full;
}

template <typename T>
void VectorStats<T>::setBufferFullFalse() {
    _element = 0;
    _buffer_full = false;
    _data_sorted = false;
    _data_ordered = false;
}

template <typename T>
int VectorStats<T>::getOutliers(int8_t deviations) const {
    float stdDev = getStdDev();
    float mean = getAverage();

    int outlier_count = 0;
    for (T value : _data_array) {
        if (std::abs(value - mean) > (stdDev * deviations)) {
            outlier_count++;
        }
    }
    //int outlier_count = std::count_if(_data_array.begin(), _data_array.end(), [mean, stdDev, deviations](int n){ return std::abs(n - mean) > (stdDev * deviations); });
    return outlier_count;
}

template <typename T>
int VectorStats<T>::getLeftSkew(int8_t deviations) const {
    if (!_data_ordered) {
        return -1;
    }

    float sum = std::accumulate(_data_array.begin() + _mid_element, _data_array.end(), 0.0);
    float mean = sum / (_size - _mid_element);
    float variance = std::inner_product(_data_array.begin() + _mid_element, _data_array.end(), _data_array.begin() + 
        _mid_element, 0.0, std::plus<float>(), [mean](float x, float y) { return (x - mean) * (y - mean); }) / (_size - _mid_element);
    float stdDev = std::sqrt(variance);

    // int skew_count = 0;
    // for (int i = 0; i < _size; ++i) {
    //     if (abs(_data_array[i] - mean) > (stdDev * deviations)) {
    //         skew_count++;
    //     } else if (skew_count > 0) {
    //         float skew_sum = std::accumulate(_data_array.begin(), _data_array.begin() + skew_count, 0.0);
    //         float skew_mean = skew_sum / skew_count;
    //         if (skew_mean < mean) {
    //             skew_count *= -1;
    //         }
    //         break;
    //     } else {break;}
    // }
    // return skew_count;

    int skew_count = 0;
    for (T value : _data_array) {
        if (std::abs(value - mean) > (stdDev * deviations)) {
            skew_count++;
        } else if (skew_count > 0) {
            float skew_sum = std::accumulate(_data_array.begin(), _data_array.begin() + skew_count, 0.0);
            float skew_mean = skew_sum / skew_count;
            if (skew_mean < mean) {
                skew_count *= -1;
            }
            break;
        } else {break;}
    }
    return skew_count;
}

template <typename T>
float VectorStats<T>::getSlope() const {
    if (!_data_ordered) {
        return -1;
    }
    
    float x_avg = (1 + _size) / 2;
    float y_avg = getAverage();
    float numerator = 0.0;
    float denominator = 0.0;    

    for (int i = 0; i < _size; ++i) {
        numerator += (i + 1 - x_avg) * (_data_array[i] - y_avg);
        denominator += std::pow(i + 1 - x_avg, 2);
    }
    return numerator / denominator;
}


#endif
