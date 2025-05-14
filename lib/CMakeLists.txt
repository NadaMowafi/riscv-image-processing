add_library(lib STATIC  
            src/Rotate.cpp
            src/BilateralFilter.cpp
            #please add any other source files you need to compile here
            # e.g. src/other_file.cpp
            )

target_include_directories(lib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)