cmake_minimum_required(VERSION 3.10)

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

#Add the cpp file that would run the test here
add_executable(main_test filters_test.cpp)

##################################################



target_link_libraries(main_test PUBLIC tests models UtilsLib)  