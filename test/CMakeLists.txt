find_package(CPPUNIT REQUIRED)
enable_testing()

add_subdirectory(config)
add_subdirectory(data)

set(TEST_SRC
        Tests.h
        )

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

add_library(test_lib ${TEST_SRC})

# *** EXECUTABLE RDMA_TESTS ***
add_executable(rdma_tests TestRunner.cc)

add_test(NAME rdma_tests COMMAND ${EXECUTABLE_OUTPUT_PATH}/rdma_tests)
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        DEPENDS rdma_tests)

SET_TARGET_PROPERTIES(rdma_tests PROPERTIES
        PREFIX ""
        LINK_FLAGS "-Wl,--no-as-needed"
        )

target_link_libraries(rdma_tests
        ${CPPUNIT_LIBRARY}
        test_config
        test_data
        )

target_link_libraries(test_lib
        ${CPPUNIT_LIBRARY}
        test_config
        test_data
        )

# *** EXECUTABLE RDMA_TEST ***
add_executable(rdma_test Main.cc)
target_link_libraries(rdma_test
        test_lib
        )


