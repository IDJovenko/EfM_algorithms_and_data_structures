#include "DisjointSets.hpp"

#include <limits>

std::unique_ptr<DisjointSets> DisjointSets::makeDisjointSets(size_t baseSize,
                                                             size_t testSize,
                                                             int startVal) {
  return std::unique_ptr<DisjointSets>(
      new DisjointSets(baseSize, testSize, startVal));
}

void DisjointSets::shuffleAll() {
  static auto mt = std::mt19937{std::random_device{}()};
  // shuffle separately
  std::shuffle(baseSet.begin(), baseSet.end(), mt);
  std::shuffle(testSet.begin(), testSet.end(), mt);

  // shuffle between each other
  for (size_t i = 0; i < baseSet.size(); ++i) {
    std::swap(baseSet[i],
              testSet[RandomNumber(0, static_cast<int>(testSet.size() - 1))]);
  }
  for (size_t i = 0; i < testSet.size(); ++i) {
    std::swap(testSet[i],
              baseSet[RandomNumber(0, static_cast<int>(baseSet.size() - 1))]);
  }
}

int DisjointSets::getNextTestValue() {
  if (testSet.empty()) {
    throw std::out_of_range("Test set is empty");
  }

  if (nextTestIndex >= testSet.size()) {
    nextTestIndex = 0;
  }
  return testSet[nextTestIndex++];
}

int DisjointSets::getNextInitValue() {
  if (baseSet.empty()) {
    throw std::out_of_range("Base set is empty");
  }

  if (nextInitIndex >= baseSet.size()) {
    nextInitIndex = 0;
  }
  return baseSet[nextInitIndex++];
}

DisjointSets::DisjointSets(size_t baseSize, size_t testSize, int startVal)
    : baseSet(baseSize),
      testSet(testSize),
      minVal(startVal),
      maxVal(startVal + static_cast<int>(baseSize + testSize)) {
  if (static_cast<size_t>(std::numeric_limits<int>::max()) <
      baseSize + testSize + static_cast<size_t>(startVal)) {
    throw std::invalid_argument(
        "Sum of startVal, baseSize and testSize exceeds maximum value for int");
  }
  baseSet.resize(baseSize);
  testSet.resize(testSize);

  for (size_t i = 0; i < baseSize; ++i) {
    baseSet[i] = RandomNumber(minVal, maxVal);
  }
  for (size_t i = 0; i < testSize; ++i) {
    testSet[i] = RandomNumber(minVal, maxVal);
  }
};

int DisjointSets::RandomNumber(int min, int max) {
  std::uniform_int_distribution<int> dist{min, max};
  return dist(gen);
}
