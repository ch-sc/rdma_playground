set(RDMA_UTILS_SRC
        file.cc
        Logging.h) # Adding headers required for portability reasons http://voices.canonical.com/jussi.pakkanen/2013/03/26/a-list-of-common-cmake-antipatterns/
add_library(rdma_utils ${RDMA_UTILS_SRC})
target_include_directories(rdma_utils PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
