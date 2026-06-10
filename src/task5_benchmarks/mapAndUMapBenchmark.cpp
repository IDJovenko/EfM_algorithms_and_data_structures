#include <benchmark/benchmark.h>

#include <map>
#include <unordered_map>

#include "BenchFixture.hpp"

template <typename Container>
using MapLikeBenchFixture =
    BenchFixture<Container, TrivialPairGenerator<int, int>>;

BENCHMARK_TEMPLATE_DEFINE_F(MapLikeBenchFixture, MapInsert,
                            std::map<int, int>)(benchmark::State& state) {
  for (auto _ : state) {
    state.PauseTiming();
    auto x = data->getNextTestValue();
    state.ResumeTiming();

    auto inserted = testContainer.insert(std::make_pair(x, x));

    state.PauseTiming();
    testContainer.erase(inserted.first);  // to avoid increasing the map size.
    state.ResumeTiming();
  }
  state.SetComplexityN(state.range(0));
}

BENCHMARK_TEMPLATE_DEFINE_F(MapLikeBenchFixture, UnorderedMapInsert,
                            std::unordered_map<int, int>)
(benchmark::State& state) {
  for (auto _ : state) {
    state.PauseTiming();
    auto x = data->getNextTestValue();
    state.ResumeTiming();

    auto inserted = testContainer.insert(std::make_pair(x, x));

    state.PauseTiming();
    testContainer.erase(inserted.first);  // to avoid increasing the map size.
    state.ResumeTiming();
  }
  state.SetComplexityN(state.range(0));
}

BENCHMARK_TEMPLATE_DEFINE_F(MapLikeBenchFixture, MapFindByKey,
                            std::map<int, int>)
(benchmark::State& state) {
  for (auto _ : state) {
    state.PauseTiming();
    auto x = data->getNextTestValue();
    state.ResumeTiming();

    auto found = testContainer.find(x);
    benchmark::DoNotOptimize(found);
  }
  state.SetComplexityN(state.range(0));
}

BENCHMARK_TEMPLATE_DEFINE_F(MapLikeBenchFixture, UnorderedMapFindByKey,
                            std::unordered_map<int, int>)
(benchmark::State& state) {
  for (auto _ : state) {
    state.PauseTiming();
    auto x = data->getNextTestValue();
    state.ResumeTiming();

    auto found = testContainer.find(x);
    benchmark::DoNotOptimize(found);
  }
  state.SetComplexityN(state.range(0));
}

BENCHMARK_TEMPLATE_DEFINE_F(MapLikeBenchFixture, MapEraseByKey,
                            std::map<int, int>)(benchmark::State& state) {
  for (auto _ : state) {
    state.PauseTiming();
    auto x = data->getNextInitValue();
    state.ResumeTiming();

    auto erased = testContainer.erase(x);

    state.PauseTiming();
    testContainer.insert(
        std::make_pair(x, x));  // to avoid decreasing the map size.
    benchmark::DoNotOptimize(erased);
    state.ResumeTiming();
  }
  state.SetComplexityN(state.range(0));
}

BENCHMARK_TEMPLATE_DEFINE_F(MapLikeBenchFixture, UnorderedMapEraseByKey,
                            std::unordered_map<int, int>)
(benchmark::State& state) {
  for (auto _ : state) {
    state.PauseTiming();
    auto x = data->getNextInitValue();
    state.ResumeTiming();

    auto erased = testContainer.erase(x);

    state.PauseTiming();
    testContainer.insert(std::make_pair(x, x));
    benchmark::DoNotOptimize(erased);
    state.ResumeTiming();
  }
  state.SetComplexityN(state.range(0));
}

// Register benchmarks with the parameter range(0) - the initial map size.

BENCHMARK_REGISTER_F(MapLikeBenchFixture, MapInsert)
    ->RangeMultiplier(2)
    ->Ranges({{1 << 12, 1 << 22}})
    ->Complexity();  // Enable complexity assessment

BENCHMARK_REGISTER_F(MapLikeBenchFixture, UnorderedMapInsert)
    ->RangeMultiplier(2)
    ->Ranges({{1 << 12, 1 << 22}})
    ->Complexity();

BENCHMARK_REGISTER_F(MapLikeBenchFixture, MapFindByKey)
    ->RangeMultiplier(2)
    ->Ranges({{1 << 12, 1 << 22}})
    ->Complexity();

BENCHMARK_REGISTER_F(MapLikeBenchFixture, UnorderedMapFindByKey)
    ->RangeMultiplier(2)
    ->Ranges({{1 << 12, 1 << 22}})
    ->Complexity();

BENCHMARK_REGISTER_F(MapLikeBenchFixture, MapEraseByKey)
    ->RangeMultiplier(2)
    ->Ranges({{1 << 12, 1 << 22}})
    ->Complexity();

BENCHMARK_REGISTER_F(MapLikeBenchFixture, UnorderedMapEraseByKey)
    ->RangeMultiplier(2)
    ->Ranges({{1 << 10, 1 << 22}})
    ->Complexity();
