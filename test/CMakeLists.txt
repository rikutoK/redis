enable_testing()
find_package(GTest CONFIG REQUIRED)

file(GLOB_RECURSE TEST_SOURCE_FILES CONFIGURE_DEPENDS *.cpp)
add_executable(tests ${TEST_SOURCE_FILES})
target_include_directories(tests PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_link_libraries(tests PRIVATE redis_server_lib)
target_link_libraries(tests PRIVATE GTest::gmock_main GTest::gmock GTest::gtest)

include(GoogleTest)
gtest_discover_tests(tests)
