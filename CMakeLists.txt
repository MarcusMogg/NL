cmake_minimum_required(VERSION 3.5)

project (NL VERSION 0.0.0)

set(CMAKE_GENERATOR "Unix Makefiles" CACHE INTERNAL "" FORCE)

set(SOURCES
    src/sync/condition.cc
    src/sync/wait_group.cc
    src/thread/current_thread.cc
    src/thread/thread.cc
    src/thread/thread_pool.cc
    src/log/log_stream.cc
    src/time/date.cc 
    src/time/time_zone.cc 
)

set(CXX_FLAGS
 -g
 -Wall
 -std=c++11
)

add_library(${PROJECT_NAME} STATIC ${SOURCES}) 

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

# TEST
# Download and Build Google Test
add_subdirectory(3rd_party/google-test)
# enable CTest testing
enable_testing()
add_subdirectory(test)


