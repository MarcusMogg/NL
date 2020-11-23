project (test)

set(TEST_SOURCES
    test_all.cc
    # sync/mutex_test.cc
    # sync/cond_test.cc
    # sync/wg_test.cc
    thread/thread_pool_test.cc
)

add_executable(unit_tests ${TEST_SOURCES})

target_link_libraries(unit_tests
    ${CMAKE_PROJECT_NAME}
    GTest::GTest 
    GTest::Main
)

add_test(test_all unit_tests)