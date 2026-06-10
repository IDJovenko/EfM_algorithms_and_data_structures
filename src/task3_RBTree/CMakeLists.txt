find_package(Threads REQUIRED)
find_package(GTest CONFIG REQUIRED)

add_executable(task3_RBTree_test test/RBTreeTest.cpp)

target_compile_features(task3_RBTree_test PRIVATE cxx_std_20)

target_include_directories(task3_RBTree_test PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/RBTree_implementation
)

target_link_libraries(task3_RBTree_test PRIVATE
  GTest::gtest
  GTest::gtest_main
  Threads::Threads
)

include(GoogleTest)
gtest_discover_tests(task3_RBTree_test)
