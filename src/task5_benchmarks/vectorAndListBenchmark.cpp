#include <benchmark/benchmark.h>

#include <iterator>
#include <list>
#include <vector>

#include "BenchFixture.hpp"

BENCHMARK_TEMPLATE_DEFINE_F(BenchFixture, VectorInsertMiddle,
                            std::vector<int>)(benchmark::State& state) {
  for (auto _ : state) {
    state.PauseTiming();
    auto x = data->getNextTestValue();
    auto middleIt = testContainer.begin() + testContainer.size() / 2;
    state.ResumeTiming();

    auto inserted = testContainer.insert(middleIt, x);

    state.PauseTiming();
    testContainer.erase(inserted);  // to avoid increasing the vector size.
    benchmark::DoNotOptimize(inserted);
    state.ResumeTiming();
  }

  state.SetComplexityN(state.range(0));
}

BENCHMARK_TEMPLATE_DEFINE_F(BenchFixture, ListInsertMiddle,
                            std::list<int>)(benchmark::State& state) {
  for (auto _ : state) {
    state.PauseTiming();
    auto x = data->getNextTestValue();
    auto middleIt = std::next(testContainer.begin(), testContainer.size() / 2);
    state.ResumeTiming();

    auto inserted = testContainer.insert(middleIt, x);

    state.PauseTiming();
    testContainer.erase(inserted);  // to avoid increasing the list size.
    benchmark::DoNotOptimize(inserted);
    state.ResumeTiming();
  }

  state.SetComplexityN(state.range(0));
}

BENCHMARK_TEMPLATE_DEFINE_F(BenchFixture, VectorEraseMiddle,
                            std::vector<int>)(benchmark::State& state) {
  for (auto _ : state) {
    state.PauseTiming();
    auto middleIt = std::next(testContainer.begin(), testContainer.size() / 2);
    auto x = *middleIt;
    state.ResumeTiming();

    auto erased = testContainer.erase(middleIt);

    state.PauseTiming();
    testContainer.insert(middleIt, x);  // to avoid decreasing the list size.
    benchmark::DoNotOptimize(erased);
    state.ResumeTiming();
  }

  state.SetComplexityN(state.range(0));
}

BENCHMARK_TEMPLATE_DEFINE_F(BenchFixture, ListEraseMiddle,
                            std::list<int>)(benchmark::State& state) {
  for (auto _ : state) {
    state.PauseTiming();
    auto middleIt = std::next(testContainer.begin(), testContainer.size() / 2);
    auto x = *middleIt;
    state.ResumeTiming();

    auto nextIt = testContainer.erase(middleIt);

    state.PauseTiming();
    testContainer.insert(nextIt, x);  // to avoid decreasing the list size.
    benchmark::DoNotOptimize(nextIt);
    state.ResumeTiming();
  }

  state.SetComplexityN(state.range(0));
}

auto range = benchmark::CreateDenseRange(7000, 20000, /*step=*/1000);
static void Linspace(benchmark::Benchmark* b) {
  for (auto x : range) b->Arg(x);
}

BENCHMARK_REGISTER_F(BenchFixture, VectorInsertMiddle)
    ->Apply(Linspace)
    ->Complexity();  // Enable complexity assessment

BENCHMARK_REGISTER_F(BenchFixture, ListInsertMiddle)
    ->Apply(Linspace)
    ->Complexity();

BENCHMARK_REGISTER_F(BenchFixture, VectorEraseMiddle)
    ->Apply(Linspace)
    ->Complexity();

BENCHMARK_REGISTER_F(BenchFixture, ListEraseMiddle)
    ->Apply(Linspace)
    ->Complexity();
