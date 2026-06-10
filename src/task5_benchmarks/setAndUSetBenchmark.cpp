#include <benchmark/benchmark.h>

#include <set>
#include <unordered_set>

#include "BenchFixture.hpp"

BENCHMARK_TEMPLATE_DEFINE_F(BenchFixture, SetInsert,
                            std::set<int>)(benchmark::State& state) {
  for (auto _ : state) {
    state.PauseTiming();
    auto x = data->getNextTestValue();
    state.ResumeTiming();

    auto inserted = testContainer.insert(x);

    state.PauseTiming();
    testContainer.erase(inserted.first);  // to avoid increasing the set size.
    state.ResumeTiming();
  }
  state.SetComplexityN(state.range(0));
}

BENCHMARK_TEMPLATE_DEFINE_F(BenchFixture, UnorderedSetInsert,
                            std::unordered_set<int>)(benchmark::State& state) {
  for (auto _ : state) {
    state.PauseTiming();
    auto x = data->getNextTestValue();
    state.ResumeTiming();

    auto inserted = testContainer.insert(x);

    state.PauseTiming();
    testContainer.erase(inserted.first);  // to avoid increasing the set size.
    state.ResumeTiming();
  }
  state.SetComplexityN(state.range(0));
}

BENCHMARK_TEMPLATE_DEFINE_F(BenchFixture, SetFindByKey,
                            std::set<int>)(benchmark::State& state) {
  for (auto _ : state) {
    state.PauseTiming();
    auto x = data->getNextTestValue();
    state.ResumeTiming();

    auto found = testContainer.find(x);
    benchmark::DoNotOptimize(found);
  }
  state.SetComplexityN(state.range(0));
}

BENCHMARK_TEMPLATE_DEFINE_F(BenchFixture, UnorderedSetFindByKey,
                            std::unordered_set<int>)(benchmark::State& state) {
  for (auto _ : state) {
    state.PauseTiming();
    auto x = data->getNextTestValue();
    state.ResumeTiming();

    auto found = testContainer.find(x);
    benchmark::DoNotOptimize(found);
  }
  state.SetComplexityN(state.range(0));
}

BENCHMARK_TEMPLATE_DEFINE_F(BenchFixture, SetEraseByKey,
                            std::set<int>)(benchmark::State& state) {
  for (auto _ : state) {
    state.PauseTiming();
    auto x = data->getNextInitValue();
    state.ResumeTiming();

    auto erased = testContainer.erase(x);

    state.PauseTiming();
    testContainer.insert(x);  // to avoid decreasing the set size.
    benchmark::DoNotOptimize(erased);
    state.ResumeTiming();
  }
  state.SetComplexityN(state.range(0));
}

BENCHMARK_TEMPLATE_DEFINE_F(BenchFixture, UnorderedSetEraseByKey,
                            std::unordered_set<int>)(benchmark::State& state) {
  for (auto _ : state) {
    state.PauseTiming();
    auto x = data->getNextInitValue();
    state.ResumeTiming();

    auto erased = testContainer.erase(x);

    state.PauseTiming();
    testContainer.insert(x);
    benchmark::DoNotOptimize(erased);
    state.ResumeTiming();
  }
  state.SetComplexityN(state.range(0));
}

// Register a benchmarks with the parameter range(0) - the initial set size.

BENCHMARK_REGISTER_F(BenchFixture, SetInsert)
    ->RangeMultiplier(2)
    ->Ranges({{1 << 12, 1 << 22}})
    ->Complexity();  // Enable complexity assessment

BENCHMARK_REGISTER_F(BenchFixture, UnorderedSetInsert)
    ->RangeMultiplier(2)
    ->Ranges({{1 << 12, 1 << 22}})
    ->Complexity();

BENCHMARK_REGISTER_F(BenchFixture, SetFindByKey)
    ->RangeMultiplier(2)
    ->Ranges({{1 << 12, 1 << 22}})
    ->Complexity();

BENCHMARK_REGISTER_F(BenchFixture, UnorderedSetFindByKey)
    ->RangeMultiplier(2)
    ->Ranges({{1 << 12, 1 << 22}})
    ->Complexity();

BENCHMARK_REGISTER_F(BenchFixture, SetEraseByKey)
    ->RangeMultiplier(2)
    ->Ranges({{1 << 12, 1 << 22}})
    ->Complexity();

BENCHMARK_REGISTER_F(BenchFixture, UnorderedSetEraseByKey)
    ->RangeMultiplier(2)
    ->Ranges({{1 << 10, 1 << 17}})
    ->Complexity();
