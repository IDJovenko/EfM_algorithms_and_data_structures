#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <set>
#include <string>
#include <vector>

#include "HashTable.hpp"

namespace ef::algo_struct {
namespace tests {

// ==================== 1. CONSTRUCTION TESTS ====================
TEST(HashTableTest, DefaultConstructorCreatesEmptyTable) {
  HashTable<int> table;

  EXPECT_EQ(table.begin(), table.end());
  EXPECT_EQ(table.getStatistics().maxCollisionLength, 0);
  EXPECT_DOUBLE_EQ(table.getStatistics().avgCollisionLength, 0.0);
}

TEST(HashTableTest, DifferentHashFunctionsWork) {
  struct CustomHash {
    size_t operator()(int x) const { return x % 10; }
  };

  HashTable<int, CustomHash> table;
  table.insert(5);
  table.insert(15);
  table.insert(25);

  auto stats = table.getStatistics();
  EXPECT_GT(stats.maxCollisionLength, 0);  // Should have collisions
}

// ==================== 2. INSERTION TESTS ====================
TEST(HashTableTest, InsertSingleElement) {
  HashTable<int> table;
  auto it = table.insert(42);

  EXPECT_NE(it, table.end());
  EXPECT_EQ(*it, 42);
  EXPECT_NE(table.find(42), table.end());
}

TEST(HashTableTest, InsertDuplicateElements) {
  HashTable<int> table;
  auto it1 = table.insert(42);
  auto it2 = table.insert(42);

  EXPECT_NE(it1, table.end());
  EXPECT_NE(it2, table.end());
  EXPECT_EQ(*it1, 42);
  EXPECT_EQ(*it2, 42);

  // Should have two elements with same value
  int count = 0;
  for (auto it = table.begin(); it != table.end(); it++) {
    if (*it == 42) count++;
  }
  EXPECT_EQ(count, 2);
}

TEST(HashTableTest, InsertManyElementsTriggersRehash) {
  HashTable<int> table;
  const int numElements = 100;

  for (int i = 0; i < numElements; ++i) {
    table.insert(i);
  }

  // Verify all elements are present
  for (int i = 0; i < numElements; ++i) {
    EXPECT_NE(table.find(i), table.end());
  }
}

TEST(HashTableTest, InsertElementsWithSameHash) {
  struct AlwaysCollideHash {
    size_t operator()(int x) const {
      return x == 0 ? 1 : static_cast<size_t>(x / x);  // 1
    }
  };

  HashTable<int, AlwaysCollideHash> table;
  const int numElements = 42;

  for (int i = 0; i < numElements; ++i) {
    table.insert(i);
  }

  auto stats = table.getStatistics();
  EXPECT_EQ(stats.maxCollisionLength, numElements);
  EXPECT_DOUBLE_EQ(stats.avgCollisionLength, 42.0);
}

// ==================== 3. FIND TESTS ====================
TEST(HashTableTest, FindExistingElement) {
  HashTable<std::string> table;
  table.insert("hello");
  table.insert("world");

  auto it = table.find("hello");
  EXPECT_NE(it, table.end());
  EXPECT_EQ(*it, "hello");
}

TEST(HashTableTest, FindNonExistingElement) {
  HashTable<int> table;
  table.insert(1);
  table.insert(2);
  table.insert(3);

  auto it = table.find(999);
  EXPECT_EQ(it, table.end());
}

TEST(HashTableTest, FindAfterRehash) {
  HashTable<int> table;

  // Insert enough elements to trigger rehash
  for (int i = 0; i < 100; ++i) {
    table.insert(i);
  }

  for (int i = 0; i < 100; ++i) {
    EXPECT_NE(table.find(i), table.end());
  }
  EXPECT_EQ(table.find(1000), table.end());
}

// ==================== 4. REMOVAL TESTS ====================
TEST(HashTableTest, RemoveByIterator) {
  HashTable<int> table;
  table.insert(1);
  table.insert(2);
  table.insert(3);

  auto it = table.find(2);
  table.remove(it);

  EXPECT_EQ(table.find(2), table.end());
  EXPECT_NE(table.find(1), table.end());
  EXPECT_NE(table.find(3), table.end());
}

TEST(HashTableTest, RemoveByValue) {
  HashTable<int> table;
  table.insert(1);
  table.insert(2);
  table.insert(2);  // Duplicate

  table.remove(2);

  // Should remove only one occurrence
  auto it = table.find(2);
  EXPECT_NE(it, table.end());

  table.remove(2);
  EXPECT_EQ(table.find(2), table.end());
  EXPECT_NE(table.find(1), table.end());
}

TEST(HashTableTest, RemoveNonExistentElement) {
  HashTable<int> table;
  table.insert(1);
  table.insert(2);

  EXPECT_NO_THROW(table.remove(999));
  // Table should remain unchanged
  EXPECT_NE(table.find(1), table.end());
  EXPECT_NE(table.find(2), table.end());
}

TEST(HashTableTest, RemoveFromCollisionBucket) {
  struct CollisionHash {
    size_t operator()(int x) const { return x % 3; }
  };

  HashTable<int, CollisionHash> table;
  // All these will go to same bucket (0)
  table.insert(3);
  table.insert(6);
  table.insert(9);
  table.insert(12);

  table.remove(6);
  EXPECT_EQ(table.find(6), table.end());
  EXPECT_NE(table.find(3), table.end());
  EXPECT_NE(table.find(9), table.end());
  EXPECT_NE(table.find(12), table.end());

  table.remove(3);
  EXPECT_EQ(table.find(3), table.end());
  EXPECT_NE(table.find(9), table.end());
  EXPECT_NE(table.find(12), table.end());
}

TEST(HashTableTest, RemoveAllElements) {
  HashTable<int> table;
  std::vector<int> elements = {1, 2, 3, 4, 5};

  for (int elem : elements) {
    table.insert(elem);
  }

  for (int elem : elements) {
    table.remove(elem);
  }

  EXPECT_EQ(table.begin(), table.end());
  for (int elem : elements) {
    EXPECT_EQ(table.find(elem), table.end());
  }
}

// ==================== 5. ITERATOR TESTS ====================
TEST(HashTableTest, BeginEndEmptyTable) {
  HashTable<int> table;
  EXPECT_EQ(table.begin(), table.end());
}

TEST(HashTableTest, IteratorIncrementOperator) {
  HashTable<int> table;
  std::set<int> inserted = {1, 2, 3, 4, 5};

  for (int x : inserted) {
    table.insert(x);
  }

  std::set<int> found;
  for (auto it = table.begin(); it != table.end(); ++it) {
    found.insert(*it);
  }

  EXPECT_EQ(inserted, found);
}

TEST(HashTableTest, PostfixIncrementOperator) {
  HashTable<int> table;
  table.insert(1);
  table.insert(2);

  auto it = table.begin();
  auto oldIt = it++;

  EXPECT_NE(oldIt, it);
  EXPECT_NE(it, table.end());
}

TEST(HashTableTest, IteratorTraversalAcrossBuckets) {
  HashTable<int> table;
  // Insert elements that will go to different buckets
  for (int i = 0; i < 100; ++i) {
    table.insert(i);
  }

  int count = 0;
  for (auto it = table.begin(); it != table.end(); ++it) {
    count++;
  }

  EXPECT_EQ(count, 100);
}

TEST(HashTableTest, MultipleIteratorsIndependent) {
  HashTable<int> table;
  table.insert(1);
  table.insert(2);
  table.insert(3);

  auto it1 = table.begin();
  auto it2 = table.begin();

  ++it1;
  EXPECT_NE(it1, it2);
  EXPECT_EQ(*it2, *table.begin());
}

// ==================== 6. STATISTICS TESTS ====================
TEST(HashTableTest, StatisticsEmptyTable) {
  HashTable<int> table;
  auto stats = table.getStatistics();

  EXPECT_DOUBLE_EQ(stats.avgCollisionLength, 0.0);
  EXPECT_EQ(stats.maxCollisionLength, 0);
}

TEST(HashTableTest, StatisticsNoCollisions) {
  HashTable<int> table;

  // Insert elements with perfect distribution
  for (int i = 0; i < 20; ++i) {
    table.insert(i);
  }

  auto stats = table.getStatistics();
  EXPECT_DOUBLE_EQ(stats.avgCollisionLength, 1.0);
  EXPECT_EQ(stats.maxCollisionLength, 1);
}

TEST(HashTableTest, StatisticsWithCollisions) {
  struct PoorHash {
    size_t operator()(int x) const {
      return x == 0 ? 1 : static_cast<size_t>(x / x);  // 1
    }
  };

  HashTable<int, PoorHash> table;
  const int numElements = 10;

  for (int i = 0; i < numElements; ++i) {
    table.insert(i);
  }

  auto stats = table.getStatistics();
  EXPECT_EQ(stats.maxCollisionLength, numElements);
  EXPECT_DOUBLE_EQ(stats.avgCollisionLength, 10.0);
}

// ==================== 7. REHASH TESTS ====================
TEST(HashTableTest, RehashTriggeredAtThreshold) {
  HashTable<int> table;
  int bucketCount = 20;  // DEFAULT_BUCKET_COUNT
  int threshold = static_cast<int>(bucketCount * 0.75);

  // Insert exactly threshold elements (no rehash yet)
  for (int i = 0; i < threshold; ++i) {
    table.insert(i);
  }

  // This should trigger rehash
  table.insert(threshold);

  // Verify all elements still accessible
  for (int i = 0; i <= threshold; ++i) {
    EXPECT_NE(table.find(i), table.end());
  }
}

TEST(HashTableTest, MultipleRehashes) {
  HashTable<int> table;
  const int numElements = 500;

  for (int i = 0; i < numElements; ++i) {
    table.insert(i);
  }

  for (int i = 0; i < numElements; ++i) {
    EXPECT_NE(table.find(i), table.end());
  }
}

// ==================== 8. EDGE CASES TESTS ====================
TEST(HashTableTest, InsertAfterRemove) {
  HashTable<int> table;
  table.insert(1);
  table.remove(1);

  auto it = table.insert(1);
  EXPECT_NE(it, table.end());
  EXPECT_NE(table.find(1), table.end());
}

TEST(HashTableTest, LargeNumberOfElements) {
  HashTable<int> table;
  const int numElements = 10000;

  for (int i = 0; i < numElements; ++i) {
    table.insert(i);
  }

  for (int i = 0; i < numElements; ++i) {
    EXPECT_NE(table.find(i), table.end());
  }
}

TEST(HashTableTest, ComplexKeyTypes) {
  struct PairHash {
    std::size_t operator()(const std::pair<int, std::string>& p) const {
      auto h1 = std::hash<int>{}(p.first);
      auto h2 = std::hash<std::string>{}(p.second);
      return h1 ^ (h2 << 1);
    }
  };

  HashTable<std::pair<int, std::string>, PairHash> table;

  auto key1 = std::make_pair(1, "one");
  auto key2 = std::make_pair(2, "two");

  table.insert(key1);
  table.insert(key2);

  EXPECT_NE(table.find(key1), table.end());
  EXPECT_NE(table.find(key2), table.end());

  auto it = table.find(key1);
  EXPECT_EQ(it->first, 1);
  EXPECT_EQ(it->second, "one");
}

}  // namespace tests
}  // namespace ef::algo_struct