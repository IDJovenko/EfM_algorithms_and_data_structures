find_package(Threads REQUIRED)
find_package(GTest CONFIG REQUIRED)

add_executable(task4_HashTable_test test/HashTableTest.cpp)

target_compile_features(task4_HashTable_test PRIVATE cxx_std_20)

target_include_directories(task4_HashTable_test PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(task4_HashTable_test PRIVATE
  GTest::gtest
  GTest::gtest_main
  Threads::Threads
)

include(GoogleTest)
gtest_discover_tests(task4_HashTable_test)
