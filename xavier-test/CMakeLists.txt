cmake_minimum_required(VERSION 3.16)
project(kpabe_test)

set(CMAKE_C_COMPILER cc)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 20)
set(CXXFLAGS "-march=x86-64-v3 -O2 -Wall -Wextra -Wno-unused-parameter")
set(CMAKE_CXX_FLAGS "${CXXFLAGS}")

# Specify RELIC include directory
#include_directories(
#    /usr/local/include/relic
#    /usr/local/include/lsss
#)

find_package(nlohmann_json 3.2.0 REQUIRED)

file(
    GLOB_RECURSE
    kpabe_sources
    ../dpvs/*
    ../keys/*
    ../kpabe/*
    ../utils.c
    ../utils.h
)

add_executable(
    test
    main_test.cpp
    ssl_utils.cpp
    kpabe_utils.cpp
    ${kpabe_sources}
)

target_link_libraries(test ssl crypto lsss relic_s gmp nlohmann_json::nlohmann_json)
