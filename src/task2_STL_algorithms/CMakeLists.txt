add_executable(task2_STL_algorithms main.cpp)

target_compile_features(task2_STL_algorithms PRIVATE cxx_std_20)

target_include_directories(task2_STL_algorithms PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
