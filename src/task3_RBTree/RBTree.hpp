#ifndef RB_TREE_HPP
#define RB_TREE_HPP

#include <cassert>     // for std::assert
#include <cstddef>     // for std::ptrdiff_t
#include <functional>  // for std::less
#include <initializer_list>
#include <iterator>  // for std::bidirectional_iterator_tag
#include <memory>    // for std::unique_ptr std::allocator_traits
#include <optional>
#include <ostream>
#include <tuple>
#include <type_traits>  // for is_same_v
#include <utility>      // for std::pair

#include "allocator_utils.h"  // for utils::AllocatorDeleter, utils::makeUniqueByAllocator

namespace ef::algo_struct {

enum Color { RED, BLACK };
enum TreeSide { LEFT, RIGHT };
constexpr TreeSide getOpposideSide(TreeSide side) noexcept {
  return side == LEFT ? RIGHT : LEFT;
}

template <class T, class Alloc>
struct Node {
 public:
  using value_type = T;
  using allocator_type =
      typename std::allocator_traits<Alloc>::template rebind_alloc<Node>;
  using NodeUniquePtr =
      std::unique_ptr<Node, utils::AllocatorDeleter<Node, allocator_type>>;

 public:
  Node() {};
  Node(T value, enum TreeSide side, enum Color color = RED,
       Node* parent = nullptr)
      : elem(value), side(side), color(color), parent(parent) {};
  Node(const Node& other) = delete;  // remove due to unique_ptr member
  Node(Node&& other) = default;
  Node& operator=(const Node& other) =
      delete;  // remove due to unique_ptr member
  Node& operator=(Node&& other) = default;

 public:
  NodeUniquePtr& getChild(enum TreeSide childSide) {
    return (childSide == LEFT ? left : right);
  }

  // @brief Returns a unique pointer to this node from its parent.
  // @pre This function must not be called on the root node.
  // Calling it on a node without a parent results in undefined behavior.
  NodeUniquePtr& getUniquePtr();
  enum TreeSide getOppositeSide() { return side == LEFT ? RIGHT : LEFT; }

  void setChild(NodeUniquePtr&& child, TreeSide childSide) {
    this->getChild(childSide) = std::move(child);
  }

  // @brief Attaches a given node to the current node
  // @details sets a given node as a child of current one,
  // updates child's parent pointer and childs's side
  void attachChild(NodeUniquePtr&& child, TreeSide childSide);

  bool isRoot() { return this->parent == nullptr; }
  static bool isBlack(Node* node) {
    return node == nullptr || node->color == BLACK;  // each leaf is black
  }
  static bool isRed(Node* node) { return !isBlack(node); }
  void replaceBy(NodeUniquePtr&& newNode);

  // @brief The function finds the extreme node (minimum or maximum)
  // in the specified subtree, starting from the given node.
  static Node* getExtremeNode(Node* node, NodeUniquePtr Node::* child);
  // @brief finds maximum node in the passed subtree
  static Node* getMinNode(Node* node) {
    return getExtremeNode(node, &Node::left);
  }
  // @brief finds minimum node in the passed subtree
  static Node* getMaxNode(Node* node) {
    return getExtremeNode(node, &Node::right);
  }

 public:
  value_type elem;
  enum TreeSide side {};  // the location of the node relative to its parent
  enum Color color {};    // color of the node
  /* Node* parent must be first declared in this class
     to provide consistency with class TreeHeader */
  Node* parent{};
  NodeUniquePtr left{};
  NodeUniquePtr right{};
};

#include "Node.ipp"

// helper struct, is used to iterators
template <class T, class Allocator>
struct TreeHeader {
 public:
  using NodeType = Node<T, Allocator>;
  static_assert(std::is_same_v<typename Allocator::value_type, T>,
                "Allocator::value_type must match the T");

 public:
  /* Node* root must be first declared in this class
     to provide consistency with class Node */
  NodeType* root{};
  NodeType* leftmost{};
  NodeType* rightmost{};

  TreeHeader()
      : root(nullptr),
        leftmost(reinterpret_cast<NodeType*>(this)),
        rightmost(reinterpret_cast<NodeType*>(this)) {};
  TreeHeader(std::nullptr_t)
      : root(nullptr),
        leftmost(reinterpret_cast<NodeType*>(this)),
        rightmost(reinterpret_cast<NodeType*>(this)) {};
};

template <class T, class Allocator, bool IsConst>
class TreeIteratorBase_;

template <class T, class Comparator = std::less<T>,
          class Allocator = std::allocator<T>>
class RBTree;

template <class T, class Comparator, class Allocator>
std::ostream& operator<<(std::ostream& os,
                         const RBTree<T, Comparator, Allocator>& t);

template <class T, class Comparator, class Allocator>
class RBTree {
 public:
  using value_type = T;
  using value_compare = Comparator;
  using size_type = size_t;
  using allocator_type =
      typename std::allocator_traits<Allocator>::template rebind_alloc<T>;
  using iterator = TreeIteratorBase_<T, Allocator, false>;
  using const_iterator = TreeIteratorBase_<T, Allocator, true>;

 private:
  using NodeType = Node<T, allocator_type>;
  using NodeUniquePtr = typename NodeType::NodeUniquePtr;

 public:
  RBTree(const value_compare& comp = Comparator(),
         const allocator_type& alloc = allocator_type())
      : root_(nullptr), header_(nullptr), comp_(comp), alloc_(alloc) {};
  RBTree(std::initializer_list<value_type> init_list,
         const value_compare& comp = Comparator(),
         const allocator_type& alloc = allocator_type())
      : root_(nullptr), header_(nullptr), comp_(comp), alloc_(alloc) {
    for (const auto& value : init_list) {
      insert(value);
    }
  }

  RBTree(const RBTree& other);
  RBTree(const RBTree& other, const allocator_type& alloc);
  RBTree(RBTree&& other)
      : root_(std::move(other.root_)),
        header_(other.header_),
        comp_(std::move(other.comp_)),
        alloc_(std::move(other.alloc_)) {
    other.updateHeader();
  };
  RBTree(RBTree&& other, const allocator_type& alloc)
      : root_(std::move(other.root_)),
        header_(other.header_),
        comp_(std::move(other.comp_)),
        alloc_(alloc) {
    other.updateHeader();
  };

  RBTree& operator=(RBTree other) {
    swap(other);
    return *this;
  }

  void swap(RBTree& other) noexcept {
    using std::swap;
    swap(root_, other.root_);
    swap(header_, other.header_);
    swap(comp_, other.comp_);
    swap(alloc_, other.alloc_);
  }
  ~RBTree() noexcept {};

  std::pair<iterator, bool> insert(const value_type& value);
  void remove(iterator nodeIter);
  bool remove(const value_type& value);
  iterator find(const value_type& value);

  iterator begin() {
    return iterator(header_.leftmost, reinterpret_cast<NodeType*>(&header_));
  }
  const_iterator begin() const {
    return const_iterator(header_.leftmost,
                          reinterpret_cast<const NodeType*>(&header_));
  }
  iterator end() {
    return iterator(reinterpret_cast<NodeType*>(&header_),
                    reinterpret_cast<NodeType*>(&header_));
  }
  const_iterator end() const {
    return const_iterator(reinterpret_cast<const NodeType*>(&header_),
                          reinterpret_cast<const NodeType*>(&header_));
  }

  template <class U, class UComparator, class UAllocator>
  friend std::ostream& operator<<(std::ostream& os,
                                  const RBTree<U, UComparator, UAllocator>& t);

 private:
  // @brief create node with given value
  // @details insert new node with given value like it's trivial binary search
  // tree, i.e. without rebalancing
  inline std::pair<iterator, bool> simpleInsert(const value_type& value);
  // @brief rebalances tree after insertion the inserted_node
  inline void fixupAfterInsert(NodeType* inserted_node);
  void handleRedParentRedUncle(NodeType* parent, NodeType* uncle,
                               NodeType* grandparent);
  inline void handleRedParentBlackUncle(NodeType* node, NodeType* parent,
                                        NodeType* grandparent);
  inline void rotate(NodeUniquePtr& rotationRoot,
                     enum TreeSide rotationDirection);
  inline void attachNewRoot(NodeUniquePtr&& newRoot);

  using RebalanceEdge = std::pair<NodeType*, TreeSide>;
  inline std::optional<RebalanceEdge> eraseAndGetRebalanceEdge(NodeType* node);
  inline std::optional<RebalanceEdge> eraseNonFullNodeAndGetRebalanceEdge(
      NodeType* node);
  inline void eraseLeafNode(NodeType* node);
  inline void eraseNodeWithOneChild(NodeType* node);
  inline std::optional<RebalanceEdge>
  eraseNodeWithTwoChildrenAndGetRebalanceEdge(NodeType* node);

  // @brief find and extract node with next key and return its parameters
  // (extract means take over and return the ownership and attach its parent to
  // its child)
  // @warning don't change any field inside of node with next key except for the
  // pointer to the right child, the ownership of which is passed on to his
  // grandparent
  inline auto extractNodeWithNextKey(NodeType* node);
  // @brief replaces the node to being deleted with the node with the next key
  // value
  inline void replaceNodeWithNext(NodeType* node,
                                  NodeUniquePtr extractedNextNode);
  inline std::optional<RebalanceEdge> constractAndGetRebalanceEdge(
      Color nodeColor, NodeType* nodeParent, TreeSide nodeSide);

  // @brief return ref to parent unique pointer that owns the passed node
  inline NodeUniquePtr& getNodeUniquePtr(NodeType* node);

  inline void fixupAfterErasing(RebalanceEdge edge);
  inline auto getRebalanceNodes(RebalanceEdge edge);
  void handleBlackBrother(NodeType* parent, NodeType* brother,
                          NodeType* closeNephew, NodeType* farNephew);
  void handleRedBrother(NodeType* parent, NodeType* brother);
  void handleBlackBrotherBlackNephews(NodeType* parent, NodeType* brother);
  inline void handleBlackBrotherRedFartherNephew(NodeType* parent,
                                                 NodeType* brother,
                                                 NodeType* farNephew);
  void handleBlackBrotherRedOnlyClosestNephew(NodeType* parent,
                                              NodeType* brother,
                                              NodeType* closeNephew);

 private:
  void setRoot(NodeUniquePtr&& newRoot) {
    root_ = std::move(newRoot);
    header_.root = root_.get();
  }
  inline void updateHeader();
  inline void updateHeaderAfterInsertion();
  inline iterator getIterator(NodeType* node);
  // Рекурсивно копирует поддерево
  NodeUniquePtr copySubtree(const NodeType* source, NodeType* parent);

 private:
  NodeUniquePtr root_{};
  TreeHeader<value_type, allocator_type>
      header_{};  // duplicates the root pointer
  // TODO comp and alloc need to be compressed using by EBO (you can use
  // boost::compressed_pair)
  value_compare comp_;
  allocator_type alloc_;
};

#include "RBTree.ipp"

template <class T, class Allocator, bool IsConst>
class TreeIteratorBase_ {
 public:
  using difference_type = std::ptrdiff_t;
  // is need const T?
  using value_type = T;
  using pointer = std::conditional_t<IsConst, const T*, T*>;
  using const_pointer = const T*;
  using reference = std::conditional_t<IsConst, const T&, T&>;
  using iterator_category = std::bidirectional_iterator_tag;

 private:
  using NodeType = Node<T, Allocator>;
  using NodeUniquePtr = typename NodeType::NodeUniquePtr;
  using HeaderType = TreeHeader<T, Allocator>;

 public:
  TreeIteratorBase_() {};
  TreeIteratorBase_(NodeType* node, NodeType* header)
      : current_(node), header_(header) {};
  TreeIteratorBase_(const NodeType* node, const NodeType* header)
      : current_(const_cast<NodeType*>(node)),
        header_(const_cast<NodeType*>(header)) {};
  TreeIteratorBase_(const TreeIteratorBase_<T, Allocator, false>& other)
      : current_(other.current_), header_(other.header_) {}
  TreeIteratorBase_(const TreeIteratorBase_<T, Allocator, true>& other)
      : current_(other.current_), header_(other.header_) {
    static_assert(IsConst,
                  "iterator can not be constructed from const_iterator");
  }
  TreeIteratorBase_& operator=(
      const TreeIteratorBase_<T, Allocator, false>& other) {
    this->current_ = other.current_;
    this->header_ = other.header_;
    return *this;
  }
  TreeIteratorBase_& operator=(
      const TreeIteratorBase_<T, Allocator, true>& other) {
    static_assert(IsConst,
                  "assignment from const_iterator to iterator is unacceptable");
    this->current_ = other.current_;
    this->header_ = other.header_;
    return *this;
  }

  reference operator*() const { return current_->elem; }
  pointer operator->()
    requires(!IsConst)
  {
    return &current_->elem;
  }
  const_pointer operator->() const { return &current_->elem; }

  TreeIteratorBase_& operator++();
  TreeIteratorBase_ operator++(int) {
    TreeIteratorBase_ tmp = *this;
    ++(*this);
    return tmp;
  }
  TreeIteratorBase_& operator--();
  TreeIteratorBase_ operator--(int) {
    TreeIteratorBase_ tmp = *this;
    --(*this);
    return tmp;
  }

 private:
  inline NodeType* data() const { return current_; }

 public:
  template <class T2, class Allocator2, bool IsConst1, bool IsConst2>
  friend bool operator==(
      const TreeIteratorBase_<T2, Allocator2, IsConst1>& lhs,
      const TreeIteratorBase_<T2, Allocator2, IsConst2>& rhs);

  template <class T2, class Allocator2, bool IsConst1, bool IsConst2>
  friend bool operator!=(
      const TreeIteratorBase_<T2, Allocator2, IsConst1>& lhs,
      const TreeIteratorBase_<T2, Allocator2, IsConst2>& rhs);

  template <class, class, class>
  friend class RBTree;

 private:
  NodeType* current_{nullptr};
  NodeType* header_{nullptr};
};

#include "TreeIteratorBase_.ipp"

}  // namespace ef::algo_struct

#endif  // RB_TREE_HPP
