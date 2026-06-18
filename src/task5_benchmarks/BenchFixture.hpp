#pragma once

#include <benchmark/benchmark.h>

#include <random>

#include "DisjointSets.hpp"

template <typename T>
struct TrivialGenerator {
  T operator()(const T& value) const { return value; }
};
template <typename KeyType, typename ValueType>
struct TrivialPairGenerator {
  using value_type = std::pair<const KeyType, ValueType>;

  value_type operator()(const KeyType& key) const {
    return {key, static_cast<ValueType>(key)};
  }
};

template <typename Container, typename ValueGenerator = TrivialGenerator<int>>
class BenchFixture : public benchmark::Fixture {
 public:
  void SetUp(const benchmark::State& state) override {
    const size_t initialSetSize = state.range(0);
    const size_t insertionValueSetSize = 1000;

    data = DisjointSets::makeDisjointSets(initialSetSize, insertionValueSetSize,
                                          0);

    // Filling the container
    ValueGenerator generator;
    std::transform(data->baseBegin(), data->baseEnd(),
                   std::inserter(testContainer, testContainer.end()),
                   [&generator](const auto& val) { return generator(val); });
  }

  void TearDown(const benchmark::State& /*state*/) override {
    data.reset();
    testContainer.clear();
  }

 protected:
  std::unique_ptr<DisjointSets> data;
  Container testContainer;
};
