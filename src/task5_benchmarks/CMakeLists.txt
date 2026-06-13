find_package(benchmark CONFIG REQUIRED)

add_executable(task5_benchmarks)
target_sources(task5_benchmarks PRIVATE
    # If Debug -> take a stub. If NOT Debug -> take the real files.
    $<$<CONFIG:DEBUG>:stubForDebug.cpp>
    $<$<NOT:$<CONFIG:Debug>>:DisjointSets.cpp;setAndUSetBenchmark.cpp;mapAndUMapBenchmark.cpp;vectorAndListBenchmark.cpp>
)

target_compile_features(task5_benchmarks PRIVATE cxx_std_20)
target_link_libraries(task5_benchmarks PRIVATE benchmark::benchmark benchmark::benchmark_main)

target_include_directories(task5_benchmarks PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
