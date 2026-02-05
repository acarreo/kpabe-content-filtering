set(KPABE_MAIN kpabe_main)
add_executable(${KPABE_MAIN} main.cpp)
target_link_libraries(${KPABE_MAIN} ${LIBRARIES} ${KPABE_LIBRARY})
target_include_directories(${KPABE_MAIN}
  PUBLIC /usr/local/include /usr/local/include/abe_lsss
  PRIVATE ${PROJECT_SOURCE_DIR}/include
)

set(KPABE_EXAMPLE kpabe_example)
add_executable(${KPABE_EXAMPLE} example.cpp)
target_link_libraries(${KPABE_EXAMPLE} ${LIBRARIES} ${KPABE_LIBRARY})
target_include_directories(${KPABE_EXAMPLE}
  PUBLIC /usr/local/include /usr/local/include/abe_lsss
  PRIVATE ${PROJECT_SOURCE_DIR}/include
)

set(KPABE_BENCH kpabe_bench)
add_executable(${KPABE_BENCH} bench.cpp)
target_link_libraries(${KPABE_BENCH} ${LIBRARIES} ${KPABE_LIBRARY})
target_include_directories(${KPABE_BENCH}
  PUBLIC /usr/local/include /usr/local/include/abe_lsss
  PRIVATE ${PROJECT_SOURCE_DIR}/include
)

