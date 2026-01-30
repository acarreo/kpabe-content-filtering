set(KPABE_MAIN kpabe_main)
add_executable(${KPABE_MAIN} main.cpp)
target_link_libraries(${KPABE_MAIN} ${LIBRARIES} ${KPABE_LIB_NAME})
target_include_directories(${KPABE_MAIN}
  PUBLIC /usr/local/include /usr/local/include/abe_lsss
  PRIVATE ${PROJECT_SOURCE_DIR}/include
)
# set_target_properties(${KPABE_MAIN} PROPERTIES
#         # some properties that successfully get applied here
#         RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# ============================================================
# Some examples and a small bench (Enable by default)
# ============================================================

## if(BUILD_EXAMPLES)
##     add_executable(kpabe_example examples/example.cpp)
##     target_link_libraries(kpabe_example ${KPABE_LIB_NAME})
## 
##     add_executable(kpabe_bench examples/bench.cpp)
##     target_link_libraries(kpabe_bench ${KPABE_LIB_NAME})
## endif()

