set(RDMA_CONFIG_SRC
  Config.h
  Config.cc
  ) # Adding headers required for portability reasons http://voices.canonical.com/jussi.pakkanen/2013/03/26/a-list-of-common-cmake-antipatterns/
add_library(rdma_config ${RDMA_CONFIG_SRC})
target_include_directories(rdma_config PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

#target_link_libraries(rdma_utils)