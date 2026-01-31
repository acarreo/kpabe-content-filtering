include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)
macro (test TEST_NAME CUSTOM_TARGET_NAME TEST_FILE)
    add_executable(${TEST_NAME} ${TEST_FILE})
    target_link_libraries(${TEST_NAME} ${LIBRARIES} GTest::gtest_main ${KPABE_LIBRARY})
    target_include_directories(${TEST_NAME}
      PUBLIC /usr/local/include /usr/local/include/abe_lsss
      PRIVATE ${PROJECT_SOURCE_DIR}/include
    )
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    add_custom_target(${CUSTOM_TARGET_NAME} COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME})
endmacro()


test(test_abe abe test_abe.cpp)
# test(test_xxx xxx test_xxx.cpp)

add_custom_target(test COMMAND
      ${CMAKE_CURRENT_BINARY_DIR}/test_abe
      #  &&  ${CMAKE_CURRENT_BINARY_DIR}/test_xxx
)
