project (test)

set(TEST_SOURCES
    sync/mutex_test.cc
)

add_executable(unit_tests ${TEST_SOURCES})

target_link_libraries(unit_tests
    ${CMAKE_PROJECT_NAME}
    GTest::GTest 
    GTest::Main
)

add_test(test_all unit_tests)