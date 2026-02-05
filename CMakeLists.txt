cmake_minimum_required(VERSION 3.10)
project(KPABE_DPVS VERSION 1.0 LANGUAGES C CXX)

set(KPABE_LIBRARY "kpabe_dpvs")

# Project compilation options
option(TEST "Build the tests" ON)
option(POC "Build the Proof of Concept program " OFF)
option(EXAMPLE "Build the Proof of Concept program " ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -Wno-dev")

set(SOURCE_FILES "")
set(PUBLIC_HEADER "")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Force link with full path
cmake_policy(SET CMP0060 NEW)
cmake_policy(SET CMP0003 NEW)

# Macro definition
#################
macro (add_sources)
    # Find the relative path from last call to project command to current dir
    file (RELATIVE_PATH _relPath "${PROJECT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
    # Iterate over argument
    foreach (_src ${ARGN})
        if (_relPath)
            # If relative path found add the file with the relative path
            list (APPEND SOURCE_FILES "${_relPath}/${_src}")
        else()
            list (APPEND SOURCE_FILES "${_src}")
        endif()
    endforeach()
    # Propagate SRCS to parent directory
    set(SOURCE_FILES ${SOURCE_FILES} PARENT_SCOPE)
endmacro()

macro (add_header)
    # Find the relative path from last call to project command to current dir
    file (RELATIVE_PATH _relPath "${PROJECT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
    # Iterate over argument
    foreach (_src ${ARGN})
        if (_relPath)
            # If relative path found add the file with the relative path
            list (APPEND PUBLIC_HEADER "${_relPath}/${_src}")
        else()
            list (APPEND PUBLIC_HEADER "${_src}")
        endif()
    endforeach()
    # Propagate SRCS to parent directory
    set(PUBLIC_HEADER ${PUBLIC_HEADER} PARENT_SCOPE)
endmacro()
#################

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

add_subdirectory(include)
add_subdirectory(src)


# ============================================================
# Library
# ============================================================

add_library(${KPABE_LIBRARY} SHARED ${SOURCE_FILES})
target_link_libraries(${KPABE_LIBRARY} PUBLIC ${LIBRARIES})
target_include_directories(${KPABE_LIBRARY}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/kpabe_dpvs>
        /usr/local/include
        /usr/local/include/abe_lsss
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)


if (TEST)
    add_subdirectory(tests)
endif()

# if (POC)
#     add_subdirectory(poc)
# endif()

if (EXAMPLE)
    add_subdirectory(examples)
endif()


# ============================================================
# Install
# ============================================================

install(TARGETS ${KPABE_LIBRARY}
    EXPORT kpabe_dpvsTargets
    LIBRARY DESTINATION lib
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION include/kpabe_dpvs
    PATTERN "CMakeLists.txt" EXCLUDE
)

