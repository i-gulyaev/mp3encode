cmake_minimum_required(VERSION 3.5)

project(mp3encode)


find_path(LAME_INCLUDES lame.h ${PROJECT_SOURCE_DIR}/../lame-3.99.5/*)
#find_path(LAME_LIB NAMES libmp3lame.a PATHS ../lame-3.99.5/*)
find_path(LAME_LIB_DIR libmp3lame.a ${PROJECT_SOURCE_DIR}/../lame-3.99.5/libmp3lame/.libs)
find_path(LAME_LIB NAMES libmp3lame.a PATHS ${PROJECT_SOURCE_DIR}/../lame-3.99.5/*)
#add_library(mp3lame STATIC IMPORTED)

find_package(Threads REQUIRED)
#target_link_libraries( Threads::Threads)

message(STATUS "LAME LIB DIR detected: [${LAME_LIB_DIR}]")
message(STATUS "LAME INCLUDE DIR detected: [${LAME_INCLUDES}]")

include_directories(${LAME_INCLUDES})
link_directories(${LAME_LIB_DIR})

message("CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")
add_definitions(-Wall)
add_executable(mp3encode mp3encode.cpp wave_file_reader.cpp encoder.cpp encoding_job.cpp)
target_link_libraries(mp3encode mp3lame Threads::Threads)
set_property(TARGET mp3encode PROPERTY CXX_STANDARD 11)
