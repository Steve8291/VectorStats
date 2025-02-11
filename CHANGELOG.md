# Change Log VectorStats

## [2.0.0] - 2025-02-11
- Rewritten to allow choice of data type.
- Can now create buffer of any type of integer or floating point!
- Renamed isBufferFull() to bufferFull()
- getAverage() and getStdDev() no longer reset bufferFull() flag.
- bufferFull() is primarily to be used with getMedian()
- Added getSlope() and getOutliers() methods
- Replaced compareFirstLast() with getLeftSkew() method
- Optimized some calculations
- Updated keywords.txt and library.json
- README.md is not yet updated
- Retested speeds to find performance is most noticeably better with large buffers.

## [1.0.2] - 2025-02-07
- Fixed keywords.txt
- Speed tests show that getMedian is up to 1.5x faster than other libraries.

## [1.0.1] - 2025-02-07
- Minor readme update

## [1.0.0] - 2025-02-06
- Initial Commit

