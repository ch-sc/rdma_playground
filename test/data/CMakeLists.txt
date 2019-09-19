set(TEST_DATA_SRC
        TestData.h
        TestData.cc)

add_library(test_data ${TEST_DATA_SRC})

target_include_directories(test_data PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(test_data rdma_utils rdma_config)
