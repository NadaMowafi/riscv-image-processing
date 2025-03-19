add_library(tests STATIC 
            ref/src/BoxFilter.cpp 
            ref/src/Gaussian.cpp  
            ref/src/rotate.cpp 
            )

target_include_directories(tests
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)