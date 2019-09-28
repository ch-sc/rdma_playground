set(RDMA_DATA_SRC
        DataRepository.h
        DataStore.h
        ) # Adding headers required for portability reasons http://voices.canonical.com/jussi.pakkanen/2013/03/26/a-list-of-common-cmake-antipatterns/


find_package(OpenMP REQUIRED)
if (OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif ()

# Configure required Boost libraries

set(BOOST_ROOT "" CACHE PATH "Boost build root (useful on Windows)")
option(Boost_USE_STATIC_LIBS
        "Search for static boost libs" OFF)
option(Boost_USE_MULTITHREADED
        "Search for multithreaded boost libs" ON)
option(Boost_USE_STATIC_RUNTIME
        "Search for boost libs linked against static C++ runtime" OFF)
find_package(Boost 1.65.1 COMPONENTS filesystem system REQUIRED)


# ensure that dependant libraries not explicitly specified here
# are found by the linker:
link_directories(${Boost_LIBRARY_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
#set(LIBS ${LIBS} ${Boost_LIBRARIES})

add_library(rdma_data ${RDMA_DATA_SRC})

target_include_directories(rdma_data PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(rdma_data
        rdma_config
        rdma_utils
        ${OpenMP_LIBRARIES}
        ${Boost_LIBRARIES}
        )
#target_link_libraries(rdma_data
#        ${LIBS})