# This file is used to set up the RISC-V toolchain for CMake.
# TO correctly activate the optimization flags use the following command when configuring the CMake before building:
# cmake .. -DCMAKE_TOOLCHAIN_FILE=../riscv-toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake_minimum_required(VERSION 3.10)
enable_testing()  # Enables CTest for this project

#################################################
set(CMAKE_REQUIRED_LIBRARIES "")
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
project(riscv-image-processing)

#################################################

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#################################################

include_directories(
    ${CMAKE_SOURCE_DIR}/models
    ${CMAKE_SOURCE_DIR}/tests/ref/include
    ${CMAKE_SOURCE_DIR}/utils
    ${CMAKE_SOURCE_DIR}/lib/include
)
add_definitions(-D_USE_MATH_DEFINES)

##################################################

add_subdirectory(tests)        
add_subdirectory(models)     
add_subdirectory(utils)     
add_subdirectory(lib) 
##################################################
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--as-needed")
# Add the cpp file that would run the test here
add_executable(BoxFilterComparison examples/BoxFilterComparison.cpp)
add_executable(GaussianComparison examples/GaussianComparison.cpp)
add_executable(RotateFlipComparison examples/Rotate&FlipComparison.cpp)
add_executable(AddComparison examples/AddComparison.cpp)

##################################################

target_link_libraries(BoxFilterComparison PUBLIC tests models UtilsLib lib)
target_link_libraries(GaussianComparison PUBLIC tests models UtilsLib lib)
target_link_libraries(RotateFlipComparison PUBLIC tests models UtilsLib lib)
target_link_libraries(AddComparison PUBLIC tests models UtilsLib lib)

##################################################

# Copy the test image file to the build directory
#barb.512.pgm
configure_file(${CMAKE_SOURCE_DIR}/examples/barb.512.pgm ${CMAKE_BINARY_DIR}/barb.512.pgm COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/examples/barb.512.pgm ${CMAKE_BINARY_DIR}/Box_Vector.pgm COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/examples/barb.512.pgm ${CMAKE_BINARY_DIR}/Box_Scalar.pgm COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/examples/barb.512.pgm ${CMAKE_BINARY_DIR}/Gaussian_Vector.pgm COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/examples/barb.512.pgm ${CMAKE_BINARY_DIR}/Gaussian_Scalar.pgm COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/examples/barb.512.pgm ${CMAKE_BINARY_DIR}/Rotate_Vector.pgm COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/examples/barb.512.pgm ${CMAKE_BINARY_DIR}/Rotate_Scalar.pgm COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/examples/barb.512.pgm ${CMAKE_BINARY_DIR}/Flipping_Vector.pgm COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/examples/barb.512.pgm ${CMAKE_BINARY_DIR}/Flipping_Scalar.pgm COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/examples/barb.512.pgm ${CMAKE_BINARY_DIR}/Add_Vector.pgm COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/examples/barb.512.pgm ${CMAKE_BINARY_DIR}/Add_Scalar.pgm COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/examples/barb.512.pgm ${CMAKE_BINARY_DIR}/Bilateral_Vector.pgm COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/examples/barb.512.pgm ${CMAKE_BINARY_DIR}/Bilateral_Scalar.pgm COPYONLY)