#ifndef ALLOCATOR_UTILS_H
#define ALLOCATOR_UTILS_H

#include <memory>
#include <type_traits>

namespace ef::utils {
// Optimized deliter, inherits from Alloc (uses Empty Base Optimization)
template <class T, class Alloc>
class AllocatorDeleter : protected Alloc {
 public:
  using allocator_type = Alloc;
  using traits_type = typename std::allocator_traits<allocator_type>;
  using value_type = T;

  static_assert(std::is_same_v<typename allocator_type::value_type, value_type>,
                "Allocator::value_type must match the T");

 public:
  AllocatorDeleter() = default;
  AllocatorDeleter(const Alloc& alloc) : Alloc(alloc) {};
  void operator()(T* ptr) {
    traits_type::destroy(*this, ptr);
    traits_type::deallocate(*this, ptr, 1);
  }
};

template <class T, class Alloc, typename... Args>
auto makeUniqueByAllocator(Alloc& alloc, Args&&... args) {
  using allocator_type =
      typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
  using traits_type = typename std::allocator_traits<allocator_type>;

  allocator_type rebind_alloc = alloc;

  // TODO add try-catch block?
  T* ptr = traits_type::allocate(rebind_alloc, 1);
  traits_type::construct(rebind_alloc, ptr, std::forward<Args>(args)...);

  using deleter_type = AllocatorDeleter<T, allocator_type>;
  return std::unique_ptr<T, deleter_type>(ptr, deleter_type(rebind_alloc));
};
}  // namespace ef::utils

#endif  // ALLOCATOR_UTILS_H
