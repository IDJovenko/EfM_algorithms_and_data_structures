add_executable(task1_containers main.cpp StudentsMarkList.cpp)

target_compile_features(task1_containers PRIVATE cxx_std_20)
target_compile_definitions(task1_containers PRIVATE DATA_DIR="${DATA_DIR}")

target_include_directories(task1_containers PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
