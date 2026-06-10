#include <gtest/gtest.h>

#include <map>
#include <random>

#include "RBTree.hpp"

namespace ef::algo_struct {

TEST(RedBlackTreeTest, ConstructorWithInitializerList) {
  RBTree<int> rbtree = {1, 2, 3, 4};
  std::set<int> stl_set = {1, 2, 3, 4};

  auto my_it = rbtree.begin();
  auto stl_it = stl_set.begin();
  for (; my_it != rbtree.end(); ++my_it, ++stl_it) {
    EXPECT_TRUE(*my_it == *stl_it);
  }
}

TEST(RedBlackTreeTest, ConstructorWithEmptyInitializerList) {
  RBTree<int> rbtree = {};
  std::set<int> stl_set = {};

  auto my_it = rbtree.begin();
  auto stl_it = stl_set.begin();
  for (; my_it != rbtree.end(); ++my_it, ++stl_it) {
    EXPECT_TRUE(*my_it == *stl_it);
  }
}

TEST(RedBlackTreeTest, ConstructorCopy) {
  RBTree<int> rbtree = {1, 3, 5};
  std::set<int> stl_set = {1, 3, 5};
  RBTree<int> rbtree_copy = rbtree;
  std::set<int> stl_set_copy = stl_set;

  auto my_it = rbtree_copy.begin();
  auto stl_it = stl_set_copy.begin();
  for (; my_it != rbtree_copy.end(); ++my_it, ++stl_it) {
    EXPECT_TRUE(*my_it == *stl_it);
  }
}

TEST(RedBlackTreeTest, ConstructorMove) {
  RBTree<int> rbtree = {1, 3, 5};
  std::set<int> stl_set = {1, 3, 5};
  RBTree<int> rbtree_copy = std::move(rbtree);
  std::set<int> stl_set_copy = std::move(stl_set);

  auto my_it = rbtree_copy.begin();
  auto stl_it = stl_set_copy.begin();
  for (; my_it != rbtree_copy.end(); ++my_it, ++stl_it) {
    EXPECT_TRUE(*my_it == *stl_it);
  }
}

TEST(RedBlackTreeTest, CopyAssignmentOperator) {
  RBTree<int> rbtree1 = {1, 2, 3};
  RBTree<int> rbtree2 = {4, 5, 6};
  rbtree2 = rbtree1;

  auto it1 = rbtree1.begin();
  auto it2 = rbtree2.begin();
  for (; it1 != rbtree1.end(); ++it1, ++it2) {
    EXPECT_EQ(*it1, *it2);
  }
}

TEST(RedBlackTreeTest, MoveAssignmentOperator) {
  RBTree<int> rbtree1 = {1, 2, 3};
  RBTree<int> rbtree2;
  rbtree2 = std::move(rbtree1);

  std::set<int> expected = {1, 2, 3};
  auto my_it = rbtree2.begin();
  auto stl_it = expected.begin();
  for (; my_it != rbtree2.end(); ++my_it, ++stl_it) {
    EXPECT_EQ(*my_it, *stl_it);
  }
  EXPECT_TRUE(rbtree1.begin() == rbtree1.end());  // Moved-from state
}

TEST(RedBlackTreeTest, ConstructorWithCustomComparator) {
  RBTree<int, std::greater<int>> rbtree = {1, 2, 3, 4};
  std::set<int, std::greater<int>> stl_set = {1, 2, 3, 4};

  auto my_it = rbtree.begin();
  auto stl_it = stl_set.begin();
  for (; my_it != rbtree.end(); ++my_it, ++stl_it) {
    EXPECT_EQ(*my_it, *stl_it);  // Should be in descending order
  }
}

TEST(RedBlackTreeTest, AllocatorAwareConstructor) {
  std::allocator<int> alloc;
  RBTree<int> rbtree(std::less<int>(), alloc);
  EXPECT_NO_THROW(rbtree.insert(42));
}

TEST(RedBlackTreeTest, InsertIntoEmptyTree) {
  RBTree<int> rbtree;
  auto result = rbtree.insert(42);

  ASSERT_TRUE(result.second);
  EXPECT_EQ(*result.first, 42);
  EXPECT_EQ(rbtree.begin(), result.first);
  EXPECT_EQ(++rbtree.begin(), rbtree.end());
}

TEST(RedBlackTreeTest, InsertManyElementsCheckOrder) {
  RBTree<int> rbtree;
  std::vector<int> values = {50, 30, 70, 20, 40, 60, 80, 15,
                             25, 35, 45, 55, 65, 75, 85};
  for (int v : values) {
    rbtree.insert(v);
  }

  std::set<int> expected(values.begin(), values.end());
  auto my_it = rbtree.begin();
  auto stl_it = expected.begin();
  for (; my_it != rbtree.end(); ++my_it, ++stl_it) {
    EXPECT_EQ(*my_it, *stl_it);
  }
}

TEST(RedBlackTreeTest, InsertDescendingOrder) {
  RBTree<int> rbtree;
  for (int i = 100; i > 0; --i) {
    rbtree.insert(i);
  }

  std::set<int> expected;
  for (int i = 1; i <= 100; ++i) expected.insert(i);

  auto my_it = rbtree.begin();
  auto stl_it = expected.begin();
  for (; my_it != rbtree.end(); ++my_it, ++stl_it) {
    EXPECT_EQ(*my_it, *stl_it);
  }
}

TEST(RedBlackTreeTest, InsertAscendingOrder) {
  RBTree<int> rbtree;
  for (int i = 1; i <= 100; ++i) {
    rbtree.insert(i);
  }

  std::set<int> expected;
  for (int i = 1; i <= 100; ++i) expected.insert(i);

  auto my_it = rbtree.begin();
  auto stl_it = expected.begin();
  for (; my_it != rbtree.end(); ++my_it, ++stl_it) {
    EXPECT_EQ(*my_it, *stl_it);
  }
}

TEST(RedBlackTreeTest, InsertMultipleElements) {
  RBTree<int> rbtree;
  std::set<int> stl_set;

  rbtree.insert(1);
  rbtree.insert(2);
  rbtree.insert(2);
  rbtree.insert(3);
  rbtree.insert(3);
  rbtree.insert(3);

  stl_set.insert(1);
  stl_set.insert(2);
  stl_set.insert(2);
  stl_set.insert(3);
  stl_set.insert(3);
  stl_set.insert(3);

  auto my_it = rbtree.begin();
  auto stl_it = stl_set.begin();
  for (; my_it != rbtree.end(); ++my_it, ++stl_it) {
    EXPECT_TRUE(*my_it == *stl_it);
  }
}

TEST(RedBlackTreeTest, InsertManyRandomElements) {
  RBTree<int> rbtree;
  std::set<int> stl_set;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(1, 1 << 20);

  const int NUM_ELEMENTS = 1000;
  for (int i = 0; i < NUM_ELEMENTS; ++i) {
    int val = dis(gen);
    rbtree.insert(val);
    stl_set.insert(val);
  }

  auto my_it = rbtree.begin();
  auto stl_it = stl_set.begin();
  for (; my_it != rbtree.end(); ++my_it, ++stl_it) {
    EXPECT_EQ(*my_it, *stl_it);
  }
}

TEST(RedBlackTreeTest, RemoveByIterator) {
  RBTree<int> rbtree = {1, 2, 3, 4};
  std::set<int> stl_set = {1, 2, 3, 4};

  rbtree.remove(rbtree.begin());
  stl_set.erase(stl_set.begin());

  auto my_it = rbtree.begin();
  auto stl_it = stl_set.begin();
  for (; my_it != rbtree.end(); ++my_it, ++stl_it) {
    EXPECT_TRUE(*my_it == *stl_it);
  }
}

TEST(RedBlackTreeTest, RemoveByKey) {
  RBTree<int> rbtree = {1, 2, 3, 4};
  std::set<int> stl_set = {1, 2, 3, 4};

  rbtree.remove(2);
  stl_set.erase(2);

  auto my_it = rbtree.begin();
  auto stl_it = stl_set.begin();
  for (; my_it != rbtree.end(); ++my_it, ++stl_it) {
    EXPECT_TRUE(*my_it == *stl_it);
  }
}

TEST(RedBlackTreeTest, RemoveRoot) {
  RBTree<int> rbtree = {10, 5, 15, 3, 7, 12, 18};
  rbtree.remove(10);

  std::set<int> expected = {3, 5, 7, 12, 15, 18};
  auto my_it = rbtree.begin();
  auto stl_it = expected.begin();
  for (; my_it != rbtree.end(); ++my_it, ++stl_it) {
    EXPECT_EQ(*my_it, *stl_it);
  }
}

TEST(RedBlackTreeTest, RemoveLeafNode) {
  RBTree<int> rbtree = {10, 5, 15, 3, 7};
  rbtree.remove(3);

  std::set<int> expected = {5, 7, 10, 15};
  auto my_it = rbtree.begin();
  auto stl_it = expected.begin();
  for (; my_it != rbtree.end(); ++my_it, ++stl_it) {
    EXPECT_EQ(*my_it, *stl_it);
  }
}

TEST(RedBlackTreeTest, RemoveNodeWithOneChild) {
  RBTree<int> rbtree = {10, 5, 15, 3};
  rbtree.remove(5);

  std::set<int> expected = {3, 10, 15};
  auto my_it = rbtree.begin();
  auto stl_it = expected.begin();
  for (; my_it != rbtree.end(); ++my_it, ++stl_it) {
    EXPECT_EQ(*my_it, *stl_it);
  }
}

TEST(RedBlackTreeTest, RemoveNodeWithTwoChildren) {
  RBTree<int> rbtree = {10, 5, 15, 3, 7, 12, 18};
  rbtree.remove(15);

  std::set<int> expected = {3, 5, 7, 10, 12, 18};
  auto my_it = rbtree.begin();
  auto stl_it = expected.begin();
  for (; my_it != rbtree.end(); ++my_it, ++stl_it) {
    EXPECT_EQ(*my_it, *stl_it);
  }
}

TEST(RedBlackTreeTest, RemoveNonExistentKey) {
  RBTree<int> rbtree = {1, 2, 3};
  std::set<int> expected = {1, 2, 3};

  bool result = rbtree.remove(42);
  EXPECT_FALSE(result);

  auto my_it = rbtree.begin();
  auto stl_it = expected.begin();
  for (; my_it != rbtree.end(); ++my_it, ++stl_it) {
    EXPECT_EQ(*my_it, *stl_it);
  }
}

TEST(RedBlackTreeTest, RemoveAllElementsOneByOne) {
  RBTree<int> rbtree = {1, 2, 3, 4, 5};
  std::set<int> expected = {1, 2, 3, 4, 5};

  for (int i = 1; i <= 5; ++i) {
    rbtree.remove(i);
    expected.erase(i);

    auto my_it = rbtree.begin();
    auto stl_it = expected.begin();
    for (; my_it != rbtree.end(); ++my_it, ++stl_it) {
      EXPECT_EQ(*my_it, *stl_it);
    }
  }
  EXPECT_TRUE(rbtree.begin() == rbtree.end());
}

TEST(RedBlackTreeTest, IteratorIncrementDecrement) {
  RBTree<int> rbtree = {1, 2};
  RBTree<int>::iterator it = rbtree.begin();
  ++it;
  ASSERT_TRUE(*it == 2);
  --it;
  ASSERT_TRUE(*it == 1);
  EXPECT_EQ(it, rbtree.begin());
}

TEST(RedBlackTreeTest, ReverseIterationViaDecrement) {
  RBTree<int> rbtree = {1, 2, 3, 4, 5};
  auto it = rbtree.end();
  --it;
  EXPECT_EQ(*it, 5);
  --it;
  EXPECT_EQ(*it, 4);
  --it;
  EXPECT_EQ(*it, 3);
}

TEST(RedBlackTreeTest, IteratorComparison) {
  RBTree<int> rbtree = {1, 2, 3};
  auto it1 = rbtree.begin();
  auto it2 = rbtree.begin();
  auto it3 = rbtree.end();

  EXPECT_TRUE(it1 == it2);
  EXPECT_FALSE(it1 == it3);
  EXPECT_TRUE(it1 != it3);
}

TEST(RedBlackTreeTest, IteratorPostIncrement) {
  RBTree<int> rbtree = {1, 2, 3};
  auto it = rbtree.begin();
  auto old = it++;
  EXPECT_EQ(*old, 1);
  EXPECT_EQ(*it, 2);
}

TEST(RedBlackTreeTest, EmptyTreeBeginEnd) {
  RBTree<int> rbtree;
  EXPECT_TRUE(rbtree.begin() == rbtree.end());
}

TEST(RedBlackTreeTest, FindNonExistentElement) {
  RBTree<int> rbtree = {1, 2};
  ASSERT_TRUE(rbtree.find(0) == rbtree.end());
}

TEST(RedBlackTreeTest, FindInNonEmptyTree) {
  RBTree<int> rbtree = {3, 2, 1, 8, 7, 6, 5};

  ASSERT_TRUE(rbtree.find(4) == rbtree.end());

  auto finded = rbtree.find(7);
  ASSERT_TRUE(finded != rbtree.end());
  EXPECT_TRUE(*finded == 7);
}

TEST(RedBlackTreeTest, StringType) {
  RBTree<std::string> rbtree = {"apple", "banana", "cherry"};
  std::set<std::string> expected = {"apple", "banana", "cherry"};

  auto my_it = rbtree.begin();
  auto stl_it = expected.begin();
  for (; my_it != rbtree.end(); ++my_it, ++stl_it) {
    EXPECT_EQ(*my_it, *stl_it);
  }
}

TEST(RedBlackTreeTest, CustomStructWithComparator) {
  struct Person {
    std::string name;
    int age;
    bool operator<(const Person& other) const { return age < other.age; }
    bool operator==(const Person& other) const {
      return name == other.name && age == other.age;
    }
  };

  RBTree<Person> rbtree = {{"Alice", 30}, {"Bob", 25}, {"Charlie", 35}};
  std::set<Person> expected = {{"Alice", 30}, {"Bob", 25}, {"Charlie", 35}};

  auto my_it = rbtree.begin();
  auto stl_it = expected.begin();
  for (; my_it != rbtree.end(); ++my_it, ++stl_it) {
    EXPECT_EQ(my_it->age, stl_it->age);
  }
}

}  // namespace ef::algo_struct
