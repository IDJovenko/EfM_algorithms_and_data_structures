#include <algorithm>
#include <memory>
#include <random>
#include <vector>

// DisjointSets provides two sets of disjoint unique integers (base and test)
// filled with random values.
// It allows elements to be shuffled between the sets,
// preserving their disjointness.
class DisjointSets {
 private:
  // A helper structure for controlling constructor access.
  // This ensures that DisjointSets objects are created only via the factory
  // method. See the "PassKey" pattern.
  struct CtorAccessToken {
    explicit CtorAccessToken() = default;
  };

 public:
  // Constructor accessible only via the makeDisjointSets() factory method.
  // Accepts a CtorAccessToken to restrict access (see the PassKey idiom).
  DisjointSets(CtorAccessToken, size_t baseSize, size_t testSize, int startVal);
  // Factory method that creates DisjointSets with base (baseSize)
  // and test (testSize) sets filled with random numbers
  // in the range [startVal, startVal + baseSize + testSize].
  static std::unique_ptr<DisjointSets> makeDisjointSets(size_t baseSize,
                                                        size_t testSize,
                                                        int startVal);

  const std::vector<int>& getInit() const { return baseSet; }
  const std::vector<int>& getTest() const { return testSet; }

  // Method for shuffling elements between the base and test sets, and
  // also within each of them.
  void shuffleAll();

  auto baseBegin() { return baseSet.cbegin(); }
  auto baseEnd() { return baseSet.cend(); }

  int getNextTestValue();
  int getNextInitValue();

 private:
  int RandomNumber(int min, int max);

 private:
  std::vector<int> baseSet;
  std::vector<int> testSet;
  size_t nextTestIndex = 0;
  size_t nextInitIndex = 0;

  std::mt19937 gen{std::random_device{}()};
  int minVal;
  int maxVal;
};
