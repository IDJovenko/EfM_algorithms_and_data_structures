add_executable(task1_Containers main.cpp StudentsMarkList.cpp)

target_compile_features(task1_Containers PRIVATE cxx_std_20)
target_compile_definitions(task1_Containers PRIVATE DATA_DIR="${DATA_DIR}")

target_include_directories(task1_Containers PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
