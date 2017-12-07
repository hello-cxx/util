cmake_minimum_required(VERSION 3.1)
project(util VERSION 1.0.0)

file(GLOB HEADER_FILES *.hpp)

set(SOURCE_FILES
        src/options.cpp
        src/threading.cpp
        src/logging.cpp
        src/flags.cpp
    )

if(TARGET opencv_core)
    list(APPEND SOURCE_FILES src/config.cpp)
endif()

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES} ${HEADER_FILES})
add_library(${HELLO_NAMESPACE}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

add_subdirectory(test)