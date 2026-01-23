cmake_minimum_required(VERSION 3.10)
project(KPABE_DPVS VERSION 1.0 LANGUAGES C CXX)

set(LIBRARY_NAME "kpabe_dpvs")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall")


# ============================================================
# Dependencies
# ============================================================

find_package(OpenSSL REQUIRED)
find_library(RLC_LIBRARY NAMES relic REQUIRED)

find_library(LSSS_LIBRARY NAMES abe_lsss REQUIRED)

set(LIBRARIES
    OpenSSL::SSL
    ${LSSS_LIBRARY}
    ${RLC_LIBRARY}
    gmp
)

# ============================================================
# Sources
# ============================================================

set(SOURCE_FILES
    src/dpvs.c
    src/matrix.c
    src/vector_ec.cpp
    src/keys.cpp
    src/kpabe.cpp
    src/utils.c
)

# ============================================================
# Library
# ============================================================

add_library(${LIBRARY_NAME} SHARED ${SOURCE_FILES})

target_link_libraries(${LIBRARY_NAME} PUBLIC ${LIBRARIES})

target_include_directories(${LIBRARY_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/kpabe_dpvs>
        /usr/local/include
        /usr/local/include/abe_lsss
)


# ============================================================
# Some examples and a small bench (Enable by default)
# ============================================================

option(BUILD_EXAMPLES "Build examples" ON)

if(BUILD_EXAMPLES)
    add_executable(kpabe_main examples/main.cpp)
    target_link_libraries(kpabe_main ${LIBRARY_NAME})

    add_executable(kpabe_example examples/example.cpp)
    target_link_libraries(kpabe_example ${LIBRARY_NAME})

    add_executable(kpabe_bench examples/bench.cpp)
    target_link_libraries(kpabe_bench ${LIBRARY_NAME})
endif()


# ============================================================
# Install
# ============================================================

install(TARGETS ${LIBRARY_NAME}
    EXPORT kpabe_dpvsTargets
    LIBRARY DESTINATION lib
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION include/kpabe_dpvs
)

