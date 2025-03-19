#add any other newly add libs here
add_library(UtilsLib STATIC 
            image_reader.cpp
            image_writer.cpp
            FFT.cpp)

target_include_directories(UtilsLib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)