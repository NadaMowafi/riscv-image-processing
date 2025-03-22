add_library(models STATIC 
            Complex.cpp
            Image.cpp
            )
            
target_include_directories(models
            PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}
        )
