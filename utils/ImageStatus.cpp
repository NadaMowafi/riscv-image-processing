// ImageStatus.h
#ifndef IMAGESTATUS_H
#define IMAGESTATUS_H

enum class ImageStatus {
    SUCCESS = 0,

    // File operations
    FILE_NOT_FOUND = 100,
    FILE_OPEN_ERROR = 101,
    FILE_READ_ERROR = 102,
    FILE_WRITE_ERROR = 103,
    UNSUPPORTED_FORMAT = 104,

     // Memory operations
     MEMORY_ALLOCATION_FAILED = 200,
     NULL_POINTER = 201,
     
     // Parameter validation
     INVALID_DIMENSIONS = 300,
     INVALID_CHANNELS = 301,
     INVALID_PARAMETERS = 302,
     
     // Processing errors
     FILTER_ERROR = 400,
     CONVERSION_ERROR = 401,
     
     // Unknown/other errors
     UNKNOWN_ERROR = 999
};

#endif // IMAGESTATUS_H