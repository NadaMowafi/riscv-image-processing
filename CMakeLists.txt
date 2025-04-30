cmake_minimum_required(VERSION 3.10)
enable_testing()  # Enables CTest for this project

#################################################

project(riscv-image-processing)

#################################################

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#################################################

include_directories(
    ${CMAKE_SOURCE_DIR}/models
    ${CMAKE_SOURCE_DIR}/tests/ref/include
    ${CMAKE_SOURCE_DIR}/utils
)
add_definitions(-D_USE_MATH_DEFINES)

##################################################

add_subdirectory(tests)        
add_subdirectory(models)     
add_subdirectory(utils)     

##################################################

# Add the cpp file that would run the test here
add_executable(main_test examples/filters_test.cpp)

add_test(NAME main_test COMMAND main_test)

##################################################

target_link_libraries(main_test PUBLIC tests models UtilsLib)

##################################################

# Copy the test image file to the build directory
configure_file(${CMAKE_SOURCE_DIR}/examples/barb.512.pgm ${CMAKE_BINARY_DIR}/barb.512.pgm COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/examples/3things256.pgm ${CMAKE_BINARY_DIR}/3things256.pgm COPYONLY)
