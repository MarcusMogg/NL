cmake_minimum_required(VERSION 3.5)

set(CMAKE_GENERATOR "Unix Makefiles" CACHE INTERNAL "" FORCE)

project (NL VERSION 0.0.0)

# use following command to list all
# find $PWD/src | xargs ls -ld | grep cc|awk '{print $9}' | sed "s@$PWD/@@g"
set(SOURCES
    src/log/file_util.cc
    src/log/log_file.cc
    src/log/log_stream.cc
    src/log/logging.cc
    src/net/acceptor.cc
    src/net/address.cc
    src/net/channel.cc
    src/net/event_loop.cc
    src/net/event_loop_thread.cc
    src/net/poller.cc
    src/net/socket.cc
    src/net/socket_opts.cc
    src/net/tcp_connection.cc
    src/net/tcp_server.cc
    src/net/timer_queue.cc
    src/sync/condition.cc
    src/sync/wait_group.cc
    src/thread/current_thread.cc
    src/thread/thread.cc
    src/thread/thread_pool.cc 
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


