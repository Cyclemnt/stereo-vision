cmake_minimum_required(VERSION 3.16)
project(stereo-vision LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
