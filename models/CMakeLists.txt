add_library(models STATIC 
            complex.cpp
            image.cpp
            )
            
target_include_directories(models
            PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}
        )
