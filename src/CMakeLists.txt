enable_testing()
include(GNUInstallDirs)

add_subdirectory(message)
#add_subdirectory(proto)
#add_subdirectory(rdma)
#add_subdirectory(thread)
add_subdirectory(utils)
add_subdirectory(config)
#add_subdirectory(RPC)


file(COPY
        ${CMAKE_CURRENT_SOURCE_DIR}/config/RDMA.conf
        DESTINATION ${CMAKE_BINARY_DIR}/bin/config/
        )