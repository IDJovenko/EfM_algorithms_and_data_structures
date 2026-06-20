#include <HashTable.hpp>
#include <algorithm>
#include <cassert>
#include <iterator>

namespace ef::algo_struct {

template <typename Key, class Hash>
inline auto HashTableIterator<Key, Hash>::operator++() -> HashTableIterator& {
  ++inBucketIter;
  advance_to_next_non_empty_bucket();
  return *this;
}

template <typename Key, class Hash>
inline auto HashTableIterator<Key, Hash>::operator++(int) -> HashTableIterator {
  HashTableIterator tmp = *this;
  ++(*this);
  return tmp;
}

template <typename Key, class Hash>
inline bool HashTableIterator<Key, Hash>::operator==(
    const HashTableIterator& other) const {
  // If both iterators point to end(), then they are equal,
  // even though their bucketIter and inBucketIter may be different
  // (for example, one may be default-constructed
  // and the other the result of calling end()).
  if (bucketIter == bucketsEnd && other.bucketIter == other.bucketsEnd) {
    return true;
  }
  // Otherwise, they are equal if they point to the same element in the same
  // bucket
  return bucketIter == other.bucketIter && inBucketIter == other.inBucketIter;
}

template <typename Key, class Hash>
inline void HashTableIterator<Key, Hash>::advance_to_next_non_empty_bucket() {
  // The loop continues as long as the current bucket exists (the end has not
  // been reached)
  // And the current position within the bucket points to its end
  // (i.e., the current bucket is empty or we have already iterated over all the
  // elements in it)
  while (bucketIter != bucketsEnd && inBucketIter == bucketIter->end()) {
    ++bucketIter;
    if (bucketIter != bucketsEnd) {
      inBucketIter = bucketIter->begin();
    }
  }
}

template <typename Key, class Hash>
inline HashTable<Key, Hash>::HashTable(size_t bucket_count, const Hash& hash)
    : hash(hash) {
  if (bucket_count <= 0) {
    throw std::invalid_argument(
        "HashTable::HashTable(): bucket_count can't be less or equal zero");
  }
  buckets.resize(bucket_count);
}

template <typename Key, class Hash>
inline auto HashTable<Key, Hash>::insert(const Key& value) -> iterator {
  if (needRehash(size + 1)) {
    rehash();
  };

  auto index = indexOf(value);
  buckets[index].push_front(value);
  size++;

  return iterator(buckets[index].begin(), buckets.begin() + index,
                  buckets.end());
}

template <typename Key, class Hash>
inline auto HashTable<Key, Hash>::find(const Key& value) -> iterator {
  auto index = hash(value) % buckets.size();

  auto bucketIt = buckets.begin() + index;
  auto& bucket = *bucketIt;

  auto inBucketIter = findInBucket(bucket, value);
  if (inBucketIter == bucket.end()) {
    return end();
  }
  return iterator(inBucketIter, bucketIt, buckets.end());
}

template <typename Key, class Hash>
inline void HashTable<Key, Hash>::remove(iterator it) {
  assert(it != end() && "HashTable::remove() called with end() iterator");
  auto& bucket = *it.bucketIter;
  bucket.erase_after(prev(bucket, it.inBucketIter));
  --size;
}

template <typename Key, class Hash>
inline bool HashTable<Key, Hash>::remove(const Key& value) {
  // TODO
  // The current implementation double-passes the elements in a bucket until the
  // desired one is found. One in find() and another when prev() called in
  // another remove. With a low rehashing factor and a high-quality hash,
  // this isn't a problem, since the buckets are very small. But if you want to
  // avoid this, you should introduce private findBefore() and removeAfter()
  // methods.
  auto it = find(value);
  if (it == end()) {
    return false;
  }
  remove(it);
  return true;
}

template <typename Key, class Hash>
inline auto HashTable<Key, Hash>::begin() -> iterator {
  if (size == 0) {
    return end();
  }
  auto iter = iterator(buckets[0].begin(), buckets.begin(), buckets.end());

  if (buckets[0].empty()) {  // it's useless, but it brings clarity.
    iter.advance_to_next_non_empty_bucket();
  }
  return iter;
}

template <typename Key, class Hash>
inline auto HashTable<Key, Hash>::end() -> iterator {
  return iterator(buckets.end());
}

template <typename Key, class Hash>
inline auto HashTable<Key, Hash>::getStatistics() const noexcept
    -> HashStatistics {
  if (buckets.empty()) {
    return {0.0, 0};
  }

  size_t maxLength{0};
  size_t totalLength{0};
  size_t filledBuckets{0};
  for (const auto& bucket : buckets) {
    const size_t bucketSize = std::distance(bucket.begin(), bucket.end());
    maxLength = std::max(maxLength, bucketSize);
    totalLength += bucketSize;
    if (!bucket.empty()) {
      ++filledBuckets;
    }
  }
  return {
      filledBuckets ? totalLength / static_cast<double>(filledBuckets) : 0.0,
      maxLength};
}

template <typename Key, class Hash>
inline bool HashTable<Key, Hash>::needRehash(size_t newSize) const noexcept {
  if (buckets.empty()) {
    return false;
  }
  return static_cast<double>(newSize) / buckets.size() > rehashCoef;
}

template <typename Key, class Hash>
inline size_t HashTable<Key, Hash>::indexOf(const Key& value) const {
  return hash(value) % buckets.size();
}

template <class Key, class Hash>
inline void HashTable<Key, Hash>::rehash() {
  assert(buckets.size() != 0);

  std::vector<HashBucket<Key>> newBuckets(buckets.size() * GROWTH_FACTOR);

  for (auto& bucket : buckets) {
    while (!bucket.empty()) {
      auto element = std::move(bucket.front());
      auto newIndex = hash(element) % newBuckets.size();
      newBuckets[newIndex].push_front(std::move(element));

      bucket.pop_front();
    }
  }
  buckets = std::move(newBuckets);
}
template <typename Key, class Hash>
inline HashBucket<Key>::iterator HashTable<Key, Hash>::findInBucket(
    HashBucket<Key>& bucket, const Key& value) {
  return std::find(bucket.begin(), bucket.end(), value);
}
template <typename Key, class Hash>
inline HashBucket<Key>::iterator HashTable<Key, Hash>::prev(
    HashBucket<Key>& bucket, const typename HashBucket<Key>::iterator& it) {
  auto prev = bucket.before_begin();
  auto curr = bucket.begin();

  while (curr != it && curr != bucket.end()) {
    prev = curr;
    ++curr;
  }

  return prev;
}
}  // namespace ef::algo_struct
