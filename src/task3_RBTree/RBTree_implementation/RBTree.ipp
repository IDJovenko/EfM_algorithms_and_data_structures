#include <utility>  // for std::swap

#include "RBTree.hpp"

// • Properties(rules) of red-black trees:
// 1. Each node is either red or black.
// 2. The root is black.
// 3. Each leaf (fictitious) is black.
// 4. If a node is red, then both of its children are black.
// 5. All paths from the root to any leaf
//    contain the same number of black nodes
//    (i.e. black height is the same for each leaf)
// • Additioanal rule used in the project:
// 6. The new node is red (it can change with rebalancing,
//    which brings the tree in line with the rules).

template <class T, class Comparator, class Allocator>
inline RBTree<T, Comparator, Allocator>::RBTree(const RBTree& other)
    : RBTree(other, other.alloc_) {}

template <class T, class Comparator, class Allocator>
typename RBTree<T, Comparator, Allocator>::NodeUniquePtr
RBTree<T, Comparator, Allocator>::copySubtree(const NodeType* source,
                                              NodeType* parent) {
  if (source == nullptr) {
    return nullptr;
  }

  NodeUniquePtr new_node = utils::makeUniqueByAllocator<NodeType>(
      alloc_, source->elem, source->side, source->color, parent);

  new_node->left = copySubtree(source->left.get(), new_node.get());
  new_node->right = copySubtree(source->right.get(), new_node.get());

  return new_node;
}

template <class T, class Comparator, class Allocator>
inline RBTree<T, Comparator, Allocator>::RBTree(const RBTree& other,
                                                const allocator_type& alloc)
    : comp_(other.comp_), alloc_(alloc) {
  if (other.root_ == nullptr) {
    root_ = nullptr;
    updateHeader();
    return;
  }

  root_ = copySubtree(other.root_.get(), nullptr);
  updateHeader();
}

// TODO add noexcept if possible otherwise rm this todo
template <class T, class Comparator, class Allocator>
inline std::pair<typename RBTree<T, Comparator, Allocator>::iterator, bool>
RBTree<T, Comparator, Allocator>::insert(const value_type& value) {
  // creation the root
  if (root_ == nullptr) {
    setRoot(utils::makeUniqueByAllocator<NodeType>(
        alloc_, value, LEFT, BLACK));  // set the root as LEFT, bc it doesn't
                                       // matter which side the root is on
    header_.leftmost = root_.get();
    header_.rightmost = root_.get();
    return std::pair<iterator, bool>(getIterator(root_.get()), true);
  }

  // insertion in the right place without considering of balance
  std::pair<iterator, bool> insert_result = simpleInsert(value);
  // if a simple insertion is successful
  if (insert_result.second)
    // and if new node isn't a leaf linked with root
    if (insert_result.first.data()->parent != root_.get())
      // then rebalance the tree
      fixupAfterInsert(insert_result.first.data());

  updateHeaderAfterInsertion();
  return insert_result;
}

template <class T, class Comparator, class Allocator>
inline void RBTree<T, Comparator, Allocator>::remove(iterator node) {
  std::optional<RebalanceEdge> edgeToRebalance =
      eraseAndGetRebalanceEdge(node.data());

  if (edgeToRebalance.has_value()) {
    fixupAfterErasing(*edgeToRebalance);
  }
  updateHeader();
}

template <class T, class Comparator, class Allocator>
inline bool RBTree<T, Comparator, Allocator>::remove(const value_type& value) {
  auto it = find(value);
  if (it == end()) {
    return false;
  }

  remove(it);
  return true;
}

template <class T, class Comparator, class Allocator>
inline auto RBTree<T, Comparator, Allocator>::find(const value_type& value) ->
    typename RBTree::iterator {
  NodeType* curr = root_.get();
  TreeSide side{};  // direction of search
  while (curr != nullptr) {
    if (comp_(value, curr->elem)) {  // default: value < curr->elem
      side = LEFT;
    } else if (comp_(curr->elem, value)) {  // default: curr->elem < value
      side = RIGHT;
    } else {
      return getIterator(curr);
    }
    curr = curr->getChild(side).get();
  }
  return end();
}

// TODO rm
// template <class T, class Comparator, class Allocator>
// inline size_t RBTree<T, Comparator, Allocator>::max_size() noexcept {
//   return std::min(
//       std::numeric_limits<typename iterator::difference_type>::max() /
//           sizeof(NodeType),
//       std::allocator_traits<allocator_type>::max_size(alloc_));
// }

template <class T, class Comparator, class Allocator>
inline std::pair<typename RBTree<T, Comparator, Allocator>::iterator, bool>
RBTree<T, Comparator, Allocator>::simpleInsert(const value_type& value) {
  NodeType* curr = root_.get();
  TreeSide side;  // direction of search for the corresponding insertion place
  while (1) {
    if (comp_(value, curr->elem)) {  // default: value < curr->elem
      side = LEFT;
    } else if (comp_(curr->elem, value)) {  // default: curr->elem < value
      side = RIGHT;
    } else
      return std::pair<iterator, bool>(
          getIterator(curr), false);  // the value is already in the tree

    if (curr->getChild(side).get()) {
      curr = curr->getChild(side).get();
    } else {
      curr->getChild(side) = utils::makeUniqueByAllocator<NodeType>(
          alloc_, value, side, RED, curr);
      return std::pair<iterator, bool>(getIterator(curr->getChild(side).get()),
                                       true);
    }
  }
}

template <class T, class Comparator, class Allocator>
inline void RBTree<T, Comparator, Allocator>::fixupAfterInsert(NodeType* n) {
  assert(n != root_.get());
  assert(n->color == RED);

  if (n->parent->color == BLACK) {
    return;  // the tree already satisfies the rules
  }

  NodeType* p = n->parent;                                    // parent
  NodeType* g = p->parent;                                    // grandparent
  NodeType* u = g->getChild(getOpposideSide(p->side)).get();  // uncle
  assert(g->color == BLACK);  // red grandparent can't have red child by rule 3.
  // if the uncle is red, recolor the nodes to maintain the rules.
  if (u && u->color == RED) {  // if uncle is nullptr - it's black by rule 2.
    handleRedParentRedUncle(p, u, g);
  }
  // Otherwise, perform rotations with recolors to balance the tree.
  else {
    handleRedParentBlackUncle(n, p, g);
  }
}

template <class T, class Comparator, class Allocator>
inline void RBTree<T, Comparator, Allocator>::updateHeader() {
  header_.root = root_.get();
  if (header_.root == nullptr) {
    header_.leftmost = header_.rightmost =
        reinterpret_cast<NodeType*>(&header_);
    return;
  }

  header_.leftmost = NodeType::getMinNode(header_.root);
  header_.rightmost = NodeType::getMaxNode(header_.root);
}

template <class T, class Comparator, class Allocator>
inline void RBTree<T, Comparator, Allocator>::updateHeaderAfterInsertion() {
  header_.root = root_.get();
  if (header_.root == nullptr) {
    header_.leftmost = header_.rightmost =
        reinterpret_cast<NodeType*>(&header_);
    return;
  }
  if (header_.leftmost->parent == nullptr ||
      header_.leftmost->left != nullptr) {
    header_.leftmost = NodeType::getMinNode(header_.root);
  }
  if (header_.rightmost->parent == nullptr ||
      header_.rightmost->right != nullptr) {
    header_.rightmost = NodeType::getMaxNode(header_.root);
  }
}

template <class T, class Comparator, class Allocator>
inline typename RBTree<T, Comparator, Allocator>::iterator
RBTree<T, Comparator, Allocator>::getIterator(NodeType* node) {
  return iterator(node, reinterpret_cast<NodeType*>(&header_));
}

template <class T, class Comparator, class Allocator>
void RBTree<T, Comparator, Allocator>::handleRedParentRedUncle(
    NodeType* parent, NodeType* uncle, NodeType* grandparent) {
  parent->color = BLACK;
  uncle->color = BLACK;
  if (grandparent->isRoot())
    return;  // if grandparent is root then balancing is end, red height raises
             // (*)
  grandparent->color = RED;
  fixupAfterInsert(grandparent);  // start balancing from the grandparent,
                                  // because the great-grandparent node may turn
                                  // out to be red and rule 3 will be violated
}

template <class T, class Comparator, class Allocator>
inline void RBTree<T, Comparator, Allocator>::handleRedParentBlackUncle(
    NodeType* node, NodeType* parent, NodeType* grandparent) {
  if (node->side != parent->side) {
    auto& parentUniquePtr = grandparent->getChild(parent->side);
    auto rotationDirection = getOpposideSide(node->side);
    rotate(parentUniquePtr, rotationDirection);
    // after rotate n became the parent of p, and p - the child of n
    // so swap pointers to match the code below
    std::swap(node, parent);
  }
  assert(node->side == parent->side);

  auto& grandparentIniquePtr = getNodeUniquePtr(grandparent);
  auto rotationDirection = getOpposideSide(parent->side);
  rotate(grandparentIniquePtr, rotationDirection);
  // reverse colors to keep the black height
  std::swap(parent->color, grandparent->color);
}

#define opposite(side) ((side) == LEFT ? RIGHT : LEFT)
/* the following structure is assumed for left rotation:
----------a = n.get()
--------/ - \ -------
-------b ... c ------
------/-\ - /-\ -----
-----d...e f...g ----

and for right - symmetrically:
----------a = n.get()
--------/ - \ -------
-------c ... b ------
------/-\ - /-\ -----
-----g...f e...d ----
*/
template <class T, class Comparator, class Allocator>
void RBTree<T, Comparator, Allocator>::rotate(NodeUniquePtr& aUniquePtr,
                                              enum TreeSide side) {
  NodeType* aPtr = aUniquePtr.get();
  NodeType* cPtr = aPtr->getChild(opposite(side)).get();
  NodeType* fPtr = cPtr->getChild(side).get();

  auto tmpUniquePtr = std::move(aUniquePtr);

  auto& cUniquePtr = aPtr->getChild(opposite(side));
  if (!aPtr->isRoot()) {
    aPtr->parent->attachChild(std::move(cUniquePtr), aPtr->side);
  } else {
    attachNewRoot(std::move(cUniquePtr));
  }

  if (fPtr != nullptr) {
    auto& fUniquePtr = cPtr->getChild(side);
    aPtr->attachChild(std::move(fUniquePtr), opposite(side));
  }

  cPtr->attachChild(std::move(tmpUniquePtr), side);
}

template <class T, class Comparator, class Allocator>
inline void RBTree<T, Comparator, Allocator>::attachNewRoot(
    NodeUniquePtr&& newRoot) {
  newRoot->parent = nullptr;
  root_ = std::move(newRoot);
};

#define hasTwoChildren(node) (node)->left && (node)->right

template <class T, class Comparator, class Allocator>
inline std::optional<typename RBTree<T, Comparator, Allocator>::RebalanceEdge>
RBTree<T, Comparator, Allocator>::eraseAndGetRebalanceEdge(NodeType* node) {
  if (hasTwoChildren(node)) {
    return eraseNodeWithTwoChildrenAndGetRebalanceEdge(node);
  }

  return eraseNonFullNodeAndGetRebalanceEdge(node);
}

#define hasChild(node) (node)->left || (node)->right

template <class T, class Comparator, class Allocator>
inline std::optional<typename RBTree<T, Comparator, Allocator>::RebalanceEdge>
RBTree<T, Comparator, Allocator>::eraseNonFullNodeAndGetRebalanceEdge(
    NodeType* node) {
  auto rebalanceEdge =
      constractAndGetRebalanceEdge(node->color, node->parent, node->side);

  if (hasChild(node)) {
    eraseNodeWithOneChild(node);
  } else {
    eraseLeafNode(node);
  }
  return rebalanceEdge;
}

template <class T, class Comparator, class Allocator>
inline void RBTree<T, Comparator, Allocator>::eraseLeafNode(NodeType* node) {
  if (node->isRoot()) {
    root_.reset();
    return;
  }
  node->getUniquePtr().reset();
}

template <class T, class Comparator, class Allocator>
inline void RBTree<T, Comparator, Allocator>::eraseNodeWithOneChild(
    NodeType* node) {
  // Unique pointer to existing child
  auto& childUniquePtr = node->left != nullptr ? node->left : node->right;

  if (node->isRoot()) {
    childUniquePtr->parent = nullptr;
    root_ = std::move(childUniquePtr);
    return;
  }

  // Set the child to the node place
  // This will also delete the node since its unique pointer will be overwritten
  NodeType* parent = node->parent;
  parent->attachChild(std::move(childUniquePtr), node->side);
}

template <class T, class Comparator, class Allocator>
inline std::optional<typename RBTree<T, Comparator, Allocator>::RebalanceEdge>
RBTree<T, Comparator, Allocator>::eraseNodeWithTwoChildrenAndGetRebalanceEdge(
    NodeType* node) {
  auto [nextNodeParent, nextNodeSide, nextNodeColor, extractedNextNode,
        rawNextNode] = extractNodeWithNextKey(node);

  replaceNodeWithNext(node, std::move(extractedNextNode));

  // if the nextNode was the right child of the erased node then it becomes the
  // parent itself
  if (nextNodeParent == node) {
    nextNodeParent = rawNextNode;
  }
  return constractAndGetRebalanceEdge(nextNodeColor, nextNodeParent,
                                      nextNodeSide);
}

template <class T, class Comparator, class Allocator>
inline auto RBTree<T, Comparator, Allocator>::extractNodeWithNextKey(
    NodeType* node) {
  auto* nextNodeRaw = NodeType::getMinNode(node->right.get());
  auto& nextNode = nextNodeRaw->getUniquePtr();

  NodeType* nextNodeParent = nextNode->parent;
  TreeSide nextNodeSide = nextNode->side;
  Color nextNodeColor = nextNode->color;
  std::unique_ptr extractedNextNode = std::move(nextNode);

  assert(extractedNextNode->left == nullptr);
  // set child to parent place
  auto& nextNodeChild = extractedNextNode->right;
  nextNodeParent->attachChild(std::move(nextNodeChild), nextNodeSide);

  return std::make_tuple(nextNodeParent, nextNodeSide, nextNodeColor,
                         std::move(extractedNextNode), extractedNextNode.get());
}

template <class T, class Comparator, class Allocator>
inline void RBTree<T, Comparator, Allocator>::replaceNodeWithNext(
    NodeType* node, NodeUniquePtr extractedNextNode) {
  auto& nodePtr = getNodeUniquePtr(node);
  std::unique_ptr extractedNode = std::move(nodePtr);

  nodePtr = std::move(extractedNextNode);
  NodeType* nextNodePtr = nodePtr.get();
  nextNodePtr->parent = extractedNode->parent;
  nextNodePtr->side = extractedNode->side;
  nextNodePtr->color = extractedNode->color;

  nextNodePtr->attachChild(std::move(extractedNode->left), LEFT);
  nextNodePtr->attachChild(std::move(extractedNode->right), RIGHT);
}

template <class T, class Comparator, class Allocator>
inline std::optional<typename RBTree<T, Comparator, Allocator>::RebalanceEdge>
RBTree<T, Comparator, Allocator>::constractAndGetRebalanceEdge(
    Color nodeColor, NodeType* nodeParent, TreeSide nodeSide) {
  auto rebalanceEdge =
      (nodeColor == BLACK)
          ? std::make_optional(std::make_pair(nodeParent, nodeSide))
          : std::nullopt;  // if the removing vertex is red, rebalancing
                           // isn't need
  return rebalanceEdge;
}

template <class T, class Comparator, class Allocator>
inline typename RBTree<T, Comparator, Allocator>::NodeUniquePtr&
RBTree<T, Comparator, Allocator>::getNodeUniquePtr(NodeType* node) {
  assert(node != nullptr);

  if (node->isRoot()) {
    return root_;
  }
  return node->getUniquePtr();
}

template <class T, class Comparator, class Allocator>
inline void RBTree<T, Comparator, Allocator>::fixupAfterErasing(
    RebalanceEdge edge) {
  if (edge.first == nullptr)
    return;  // was called from root - black height has been decremented

  auto [p,   // parent
        x,   // node with lack of 1 black in height
        b,   // brother
        cN,  // closest nephew
        fN   // farther nephew
  ] = getRebalanceNodes(edge);

  if (x != nullptr &&
      x->color ==
          RED) {  // note: a leaf(=nullptr node) is always BLACK by rule 2
    x->color = BLACK;
    return;
  }

  if (NodeType::isBlack(b)) {
    handleBlackBrother(p, b, cN, fN);
  } else {
    handleRedBrother(p, b);
  }
}

template <class T, class Comparator, class Allocator>
inline auto RBTree<T, Comparator, Allocator>::getRebalanceNodes(
    RebalanceEdge edge) {
  NodeType* p = edge.first;  // parent
  NodeType* x =
      p->getChild(edge.second).get();  // node with lack of 1 black in height
  NodeType* b = p->getChild(getOpposideSide(edge.second)).get();  // brother
  NodeType* cN;  // closest nephew
  NodeType* fN;  // farther nephew
  if (b == nullptr)
    cN = fN = nullptr;
  else {
    cN = b->getChild(edge.second).get();
    fN = b->getChild(getOpposideSide(edge.second)).get();
  }

  return std::make_tuple(p, x, b, cN, fN);
}

template <class T, class Comparator, class Allocator>
inline void RBTree<T, Comparator, Allocator>::handleBlackBrother(NodeType* p,
                                                                 NodeType* b,
                                                                 NodeType* cN,
                                                                 NodeType* fN) {
  if (NodeType::isBlack(cN) && NodeType::isBlack(fN)) {
    handleBlackBrotherBlackNephews(p, b);
    return;
  }
  if (NodeType::isRed(fN)) {
    handleBlackBrotherRedFartherNephew(p, b, fN);
  } else {
    handleBlackBrotherRedOnlyClosestNephew(p, b, cN);
  }
}

template <class T, class Comparator, class Allocator>
inline void RBTree<T, Comparator, Allocator>::handleRedBrother(NodeType* p,
                                                               NodeType* b) {
  // bring about BlackBrother case
  std::swap(p->color, b->color);
  TreeSide xSide, rotationDirection;
  rotationDirection = xSide = getOpposideSide(b->side);
  rotate(getNodeUniquePtr(p), rotationDirection);

  // TODO call handleBlackbBrother instead of fixupAfterErasing
  fixupAfterErasing(
      std::make_pair(p, xSide));  // side of x didn't change after rotation
}

template <class T, class Comparator, class Allocator>
void RBTree<T, Comparator, Allocator>::handleBlackBrotherBlackNephews(
    NodeType* p, NodeType* b) {
  b->color = RED;
  if (p->color == BLACK) {
    fixupAfterErasing(std::make_pair(p->parent, p->side));
  } else {
    p->color = BLACK;
  }
}

template <class T, class Comparator, class Allocator>
inline void
RBTree<T, Comparator, Allocator>::handleBlackBrotherRedFartherNephew(
    NodeType* p, NodeType* b, NodeType* fN) {
  auto rotationDirection = getOpposideSide(b->side);
  rotate(getNodeUniquePtr(p), rotationDirection);

  b->color = p->color;
  p->color = BLACK;
  fN->color = BLACK;
}

template <class T, class Comparator, class Allocator>
void RBTree<T, Comparator, Allocator>::handleBlackBrotherRedOnlyClosestNephew(
    NodeType* p, NodeType* b, NodeType* cN) {
  // make a farest nephew red to bring about BlackBrotherRedFartherNephew case
  std::swap(b->color, cN->color);
  auto rotationDirection = b->side;
  rotate(b->getUniquePtr(), rotationDirection);

  auto newBrother = cN;
  auto newFartherNephew = b;
  handleBlackBrotherRedFartherNephew(p, newBrother, newFartherNephew);
}

// ***
// The code for outputting a tree to the console
// is taken from Arty's answer on Stack Overflow:
// https://ru.stackoverflow.com/a/1222334
static std::string ch_udia_hor = "\\-", ch_ddia_hor = "/-", ch_ver_spa = "| ";
template <class T, class Alloc>
void dump2(std::ostream& os, Node<T, Alloc> const* node,
           std::string const& rpref = "", std::string const& cpref = "",
           std::string const& lpref = "") {
  if (!node) return;
  if (node->right) {
    dump2(node->right.get(), rpref + "  ", rpref + ch_ddia_hor,
          rpref + ch_ver_spa);
  }
  os << cpref;
  if (node->color == RED) {
    os << "\033[31m" << node->elem << "\033[0m" << std::endl;
  } else {
    os << node->elem << std::endl;
  }
  if (node->left) {
    dump2(node->left.get(), lpref + ch_ver_spa, lpref + ch_udia_hor,
          lpref + "  ");
  }
}
// ***

template <class T, class Comparator, class Allocator>
std::ostream& operator<<(std::ostream& os,
                         const RBTree<T, Comparator, Allocator>& t) {
  dump2(os, t.root_.get());
  return os;
}
