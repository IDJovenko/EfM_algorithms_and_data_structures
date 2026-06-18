# STL Container Performance Comparison

## Benchmark Results (map VS unordered_map)

![mapInsert.png](/misc/images/benchmarks/map_unorderedMap/mapInsert.png)
*Insertion into map*

![unorderedMapInsert.png](/misc/images/benchmarks/map_unorderedMap/unorderedMapInsert.png)
*Insertion into unordered_map*

![mapFindByKey.png](/misc/images/benchmarks/map_unorderedMap/mapFindByKey.png)
*Search in map*

![unorderedMapFindByKey.png](/misc/images/benchmarks/map_unorderedMap/unorderedMapFindByKey.png)
*Search in unordered_map*

![mapEraseByKey.png](/misc/images/benchmarks/map_unorderedMap/mapEraseByKey.png)
*Erase by key from map*

![unorderedMapEraseByKey.png](/misc/images/benchmarks/map_unorderedMap/unorderedMapEraseByKey.png)
*Erase by key from unordered_map*

### Key Findings
`std::unordered_map` significantly outperforms `std::map` in all operations (insertion, search, erase), demonstrating the expected average `O(1)` behavior. `std::map` shows the expected logarithmic growth as container size increases.

### Practical Recommendations
Choose `std::unordered_map` if:

- Maximum speed is the priority (especially for search and insertion)
- Ordered traversal of elements is not required
- A good hash function is available (standard `std::hash<int>` is suitable)
- Predictable average constant time is more important than deterministic performance

Choose `std::map` if:
- Sorted element traversal is required
- Iterator stability is required (insertion/erasure does not invalidate other iterators)
- Worst-case lookup behavior is more important than average-case (hash table can degrade to `O(n)`)
- Container size is small (< 10K elements) and performance differences are not critical
- Strict latency requirements exist in a real-time system: map provides predictable `O(log n)` without rehashing spikes

---

## Benchmark Results (set VS unordered_set)

![setInsert.png](/misc/images/benchmarks/set_unordereSet/setInsert.png)
*Insertion into set*

![unorderedSetInsert.png](/misc/images/benchmarks/set_unordereSet/unorderedSetInsert.png)
*Insertion into unordered_set*

![setFindByKey.png](/misc/images/benchmarks/set_unordereSet/setFindByKey.png)
*Search in set*

![unorderedSetFindByKey.png](/misc/images/benchmarks/set_unordereSet/unorderedSetFindByKey.png)
*Search in unordered_set*

![setEraseByKey.png](/misc/images/benchmarks/set_unordereSet/setEraseByKey.png)
*Erase by key from set*

![unorderedSetEraseByKey.png](/misc/images/benchmarks/set_unordereSet/unorderedSetEraseByKey.png)
*Erase by key from unordered_set*

### Key Findings
Benchmark results for `std::set` and `std::unordered_set` show the same patterns as the `std::map` vs `std::unordered_map` comparison. This is expected because:

- `std::set` and `std::map` share the same red-black tree implementation (the only difference is the stored value type: key vs key-value pair)
- `std::unordered_set` and `std::unordered_map` also share the same hash table implementation

Thus, all conclusions from the previous analysis apply to the set vs unordered_set comparison:

- `std::unordered_set` is significantly faster on large data volumes
- `std::set` shows predictable logarithmic time growth
- `std::unordered_set` provides average constant-time operations
- The performance gap grows with container size

---

## Benchmark Results (vector VS list)

![vectorInsertInMidle](/misc/images/benchmarks/list_vector/vectorInsertInMiddle.png)
*Insertion into the middle of vector*

![listInsertInMidle](/misc/images/benchmarks/list_vector/listInsertInMiddle.png)
*Insertion into the middle of list*

![vectorEraseFromMidle](/misc/images/benchmarks/list_vector/vectorEraseFromMiddle.png)
*Erase from the middle of vector*

![listEraseFromMidle](/misc/images/benchmarks/list_vector/listEraseFromMiddle.png)
*Erase from the middle of list*

### Key Findings
The benchmark results demonstrate fundamentally different performance characteristics for `std::vector` and `std::list`:

- Middle insertion: `std::list` is faster across the entire size range (from 7,000 to 20,000 elements), and Google Benchmark correctly identifies the complexity as `O(1)`. A slight increase is observed, likely due to additional cache misses from poor locality, allocator behavior, and growing TLB misses.
- Middle erase: `std::list` is significantly faster than vector (approximately 10-30x), consistent with theoretical expectations.

The key factor is that `std::vector` incurs `O(n)` element movement, while `std::list` performs operations in `O(1)` given an iterator.

### Practical Recommendations

Choose `std::vector` if:
- Insertions and removals occur mostly at the end of the container (`push_back`/`pop_back`) — vector is optimal
- Middle insertion/removal frequency is low (for example, rare modifications with frequent index access)
- Data locality is important (vector stores elements contiguously, improving iteration and sequential access)
- Random access by index is required (`operator[]` is not available for list)
- Read-to-write ratio is high (for example, 100:1 or higher)

Choose `std::list` if:
- Insertions and removals occur frequently in the middle of the container
- Iterators must remain stable (insertion/erasure in list does not invalidate other iterators)
- Random access is not required — only sequential traversal is needed
