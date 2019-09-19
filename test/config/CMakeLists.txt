set(TEST_CONFIG_SRC
        TestConfig.h
        TestConfig.cc)
add_library(test_config ${TEST_CONFIG_SRC})

target_include_directories(test_config PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(test_config rdma_utils rdma_config)