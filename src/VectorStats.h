/**
 * @file VectorStats.h
 * @brief This header file contains declarations for the VectorStats class.
 * @author Steve Hambling
 * @date 2025-02-04
 */

#ifndef VECTORSTATS_H
#define VECTORSTATS_H

#include <Arduino.h>
#include <vector>
#include <algorithm>
#include <numeric>

/**
 * @class VectorStats
 * @brief Class to create C++ vector buffers for fast median, average, and standard deviation.
 */
class VectorStats {
public:
    /**
     * @brief Constructor for VectorStats.
     * @param max_buffer_size An integer value to initialize the maximum buffer size.
     */
    VectorStats(int max_buffer_size);

    /**
     * @brief Resizes and zeroes buffer.
     * @param buffer_size An integer value for new buffer size. Must be smaller than initial max_buffer_size.
     */
    void resize(int buffer_size);

    // Add sensor reading to array
    /**
     * @brief Adds value to buffer replacing oldest data first if buffer is unsorted.
     * @param value An int16_t value to be added to buffer.
     */
    void add(int16_t value);

    // Fill buffer with value.
    /**
     * @brief Fills entire buffer with value.
     * @param value An int16_t value to fill buffer.
     * Sets .isBufferFull() to true.
     */
    void fillBuffer(int16_t value);

    /**
     * @brief Calculates median of buffer data set.
     * Resets .isBufferFull() to false.
     * Changes the order of values in buffer.
     * @return The median as an int16_t
     * - Odd sized buffers are faster.
     * - Returns rounded average of center two numbers for even sized buffers.
     * - Returns -1 if buffer is not full.
     */
    int16_t getMedian();

    /**
     * @brief Calculates the average of buffer data set.
     * Resets .isBufferFull() to false.
     * Does not change order of values in buffer.
     * @return The average as a float.
     */
    float getAverage();

    /**
     * @brief Calculates the population standard deviation of buffer data set.
     * @return Standard Deviation as a float.
     * Resets .isBufferFull() to false.
     * Does not change order of values in buffer.
     */
    float getStdDev();

    /**
     * @brief Gets element from unsorted buffer.
     * @param element An integer representing the index value.
     * @return Element as int16_t.
     * - Returns -1 for all values if called on a sorted buffer.
     * Do not use after calling getSortedElement() or getMedian().
     */
    int16_t getElement(int element) const;

    /**
     * @brief Gets element from unsorted buffer.
     * @param element An integer representing the index value.
     * @return Element as int16_t.
     * Changes the order of values in buffer.
     */
    int16_t getSortedElement(int element);

    /**
     * @brief Checks state of buffer.
     * @return Boolean true if buffer is full.
     */
    bool isBufferFull() const;

    /**
     * @brief Toggles .isBufferFull() flag to false.
     * Allows overwriting of data in buffer without the overhead of zeroing it.
     */
    void setBufferFullFalse();

    /**
     * @brief Zeroes the buffer and sets .isBufferFull() to false.
     */
    void zeroBuffer();

    /**
     * @brief Compares first elements with average of last 3 to show when data normalizes.
     * Prints to serial monitor:
     * - The average of last 3 elements --> List of first elements less than avg.
     * Can be used to determine if enough time has elapsed to charge capacitor.
     * If cap delay is long enough you will see only an occasional 1 or 2 items in list.
     * Does not change order of values in buffer.
     */
    void compareFirstLast() const;

private:
    std::vector<int16_t> _data_array;
    int _max_buffer_size;
    int _size;
    int _element;
    bool _odd_parity;
    bool _buffer_full;
    bool _data_sorted;
    bool _data_ordered;

};

#endif