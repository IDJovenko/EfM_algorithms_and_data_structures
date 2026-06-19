#include "RBTree.hpp"

template <class T, class Alloc>
inline typename Node<T, Alloc>::NodeUniquePtr& Node<T, Alloc>::getUniquePtr() {
  assert(parent != nullptr &&
         "getUniquePtr() should not be called on a root node");
  return parent->getChild(this->side);
}

template <class T, class Alloc>
inline void Node<T, Alloc>::attachChild(NodeUniquePtr&& child,
                                        TreeSide childSide) {
  if (child != nullptr) {
    child->parent = this;
    child->side = childSide;
  }
  this->setChild(std::move(child), childSide);
}

template <class T, class Alloc>
inline void Node<T, Alloc>::replaceBy(Node<T, Alloc>::NodeUniquePtr&& newNode) {
  if (parent) {
    parent->setChild(std::move(newNode), side);
  }
  if (left) {
    left->parent = newNode.get();
  }
  if (right) {
    right->parent = newNode.get();
  }
}

template <class T, class Alloc>
inline Node<T, Alloc>* Node<T, Alloc>::getExtremeNode(
    Node* node, NodeUniquePtr Node::* child) {
  assert(node != nullptr);
  while (node->*child != nullptr) {
    node = (node->*child).get();
  }
  return node;
}
