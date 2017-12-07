cmake_minimum_required(VERSION 3.1)
project(util-test)

file(GLOB SOURCE_FILES *_test.cpp)
add_executable(${PROJECT_NAME} test.cpp ${SOURCE_FILES})

find_package(Boost REQUIRED COMPONENTS log program_options unit_test_framework)

target_link_libraries(${PROJECT_NAME} ${HELLO_NAMESPACE}::util
        Boost::boost Boost::log Boost::program_options Boost::unit_test_framework)