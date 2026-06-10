#include "RBTree.hpp"

template <class T, class Allocator, bool IsConst>
inline TreeIteratorBase_<T, Allocator, IsConst>&
TreeIteratorBase_<T, Allocator, IsConst>::operator++() {
  if (current_ == nullptr || current_ == header_) return *this;

  if (current_->right) {
    current_ = NodeType::getMinNode(current_->right.get());
    return *this;
  }

  NodeType* parent = current_->parent;
  while (parent && current_ == parent->right.get()) {
    current_ = parent;
    parent = parent->parent;
  }
  current_ = parent != nullptr ? parent : header_;
  return *this;
}

template <class T, class Allocator, bool IsConst>
inline TreeIteratorBase_<T, Allocator, IsConst>&
TreeIteratorBase_<T, Allocator, IsConst>::operator--() {
  if (current_ == header_) {
    current_ = (reinterpret_cast<HeaderType*>(header_))->rightmost;
    return *this;
  }

  if (current_->left) {
    current_ = NodeType::getMaxNode(current_->left.get());
    return *this;
  }

  NodeType* parent = current_->parent;
  while (parent && current_ == parent->left.get()) {
    current_ = parent;
    parent = parent->parent;
  }
  current_ = parent != nullptr ? parent : header_;
  return *this;
}
