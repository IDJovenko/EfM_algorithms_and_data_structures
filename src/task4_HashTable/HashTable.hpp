#ifndef HASHTABLE_HPP
#define HASHTABLE_HPP

#include <forward_list>
#include <functional>
#include <vector>

namespace ef::algo_struct {

template <class Key>
using HashBucket = std::forward_list<Key>;
template <typename Key, class Hash = std::hash<Key>>
class HashTable;

// TODO add a const iterator
template <typename Key, class Hash>
class HashTableIterator {
 public:
  using iterator_category = std::forward_iterator_tag;
  using iterator_concept = std::forward_iterator_tag;
  using value_type = Key;
  using difference_type = std::ptrdiff_t;
  using pointer = Key*;
  using reference = Key&;
  using const_reference = const Key&;

  HashTableIterator(HashBucket<Key>::iterator inBucketIter,
                    HashTable<Key, Hash>::BucketIterator bucketIter,
                    HashTable<Key, Hash>::BucketIterator bucketsEnd)
      : inBucketIter(inBucketIter),
        bucketIter(bucketIter),
        bucketsEnd(bucketsEnd) {};
  // ctor for end() iterator
  HashTableIterator(HashTable<Key, Hash>::BucketIterator bucketsEnd)
      : bucketIter(bucketsEnd), bucketsEnd(bucketsEnd) {};

  reference operator*() { return *inBucketIter; }
  const_reference operator*() const { return *inBucketIter; }
  pointer operator->() { return &(*inBucketIter); }

  HashTableIterator& operator++();
  HashTableIterator operator++(int);

  bool operator==(const HashTableIterator&) const;
  bool operator!=(const HashTableIterator&) const = default;

 private:
  friend class HashTable<Key, Hash>;

  void advance_to_next_non_empty_bucket();
  // points to a concreate elem in a bucket
  HashBucket<Key>::iterator inBucketIter;
  // points to a bucket
  HashTable<Key, Hash>::BucketIterator bucketIter;
  HashTable<Key, Hash>::BucketIterator bucketsEnd;
};

// HashTable are containers that store elements in no particular order, allowing
// fast retrieval of individual elements based on their value, allowing
// different elements to have equivalent values.
//
// Similar to std::unordered_multiset, but there is no guarantee that equal
// elements will be adjacent during iteration
// and the bucket node only has a next pointer, unlike standard implementations
// (GCC, Clang, MSVC), which use two pointers for navigation inside a bucket.
// Because of this feature, it requires less memory,
// but deleting an element requires more than one pass through the bucket.
template <typename Key, class Hash>  // TODO add allocator template parameter
class HashTable {
 public:
  using iterator = HashTableIterator<Key, Hash>;
  using key_type = Key;
  using value_type = Key;
  using hasher = Hash;

  struct HashStatistics {
    double avgCollisionLength;
    size_t maxCollisionLength;
  };

  static constexpr double DEFAULT_REHASH_COEF = 0.75;
  static constexpr size_t DEFAULT_BUCKET_COUNT = 20;
  static constexpr size_t GROWTH_FACTOR = 2;

  HashTable() : HashTable(DEFAULT_BUCKET_COUNT) {};
  explicit HashTable(const Hash& hash_func)
      : HashTable(DEFAULT_BUCKET_COUNT, hash_func) {};
  explicit HashTable(size_t bucket_count, const Hash& hash = Hash());

  HashTable(const HashTable&) = default;
  HashTable(HashTable&&) = default;
  HashTable& operator=(const HashTable&) = default;
  HashTable& operator=(HashTable&&) = default;

  ~HashTable() noexcept = default;

  iterator insert(const Key& value);
  iterator find(const Key& value);
  void remove(iterator it);
  bool remove(const Key& value);

  iterator begin();
  iterator end();

  HashStatistics getStatistics() const noexcept;

 private:
  friend class HashTableIterator<Key, Hash>;

  using BucketContainer = std::vector<HashBucket<Key>>;
  using BucketIterator = typename BucketContainer::iterator;

  bool needRehash(const size_t size) const noexcept;
  void rehash();

  size_t indexOf(const Key& value) const;
  HashBucket<Key>::iterator findInBucket(HashBucket<Key>& bucket,
                                         const Key& value);
  HashBucket<Key>::iterator prev(HashBucket<Key>& bucket,
                                 const typename HashBucket<Key>::iterator& it);

  BucketContainer buckets;
  // a count of Key elements
  size_t size = 0;
  // if (rehashCoef < size/buckets.size()) then rehash() will be called
  double rehashCoef = DEFAULT_REHASH_COEF;

  Hash hash{};
};
}  // namespace ef::algo_struct

#include "HashTable.ipp"

#endif  // HASHTABLE_HPP
