#include "Dependence.h"

namespace icsa {

template <typename ValueType, typename NodeType, typename ChildIteratorType>
struct DNBaseGraphTraits {
  static NodeType *getEntryNode(icsa::DependenceNode<ValueType> *N) {
    return N;
  }

  static inline ChildIteratorType child_begin(NodeType *N) {
    return N->begin();
  }

  static inline ChildIteratorType child_end(NodeType *N) { return N->end(); }
};

template <typename ValueType>
struct DNGraphTraits
    : public DNBaseGraphTraits<ValueType, DependenceNode<ValueType>,
                               typename DependenceNode<ValueType>::iterator> {
  typedef icsa::DependenceNode<ValueType> NodeType;
  typedef typename icsa::DependenceNode<ValueType>::iterator ChildIteratorType;
};

template <typename ValueType>
struct ConstDNGraphTraits
    : public DNBaseGraphTraits<
          ValueType, const DependenceNode<ValueType>,
          typename DependenceNode<ValueType>::const_iterator> {
  typedef const icsa::DependenceNode<ValueType> NodeType;
  typedef typename icsa::DependenceNode<ValueType>::const_iterator
      ChildIteratorType;
};

template <typename ValueType, typename NodeType, typename GraphType,
          typename ChildIteratorType>
struct DGBaseGraphTraits
    : public DNBaseGraphTraits<ValueType, NodeType, ChildIteratorType> {
  static NodeType *getEntryNode(icsa::DependenceGraph<ValueType> *G) {
    return G->getRootNode();
  }

  typedef icsa::DependenceBaseIterator<
      ValueType, icsa::DependenceNode<ValueType>> nodes_iterator;

  static nodes_iterator nodes_begin(GraphType *G) { return nodes_iterator(*G); }

  static nodes_iterator nodes_end(GraphType *G) {
    return nodes_iterator(*G, true);
  }

  static size_t size(GraphType *G) { return G->getNumNodes(); }
};

template <typename ValueType>
struct DGGraphTraits
    : public DGBaseGraphTraits<ValueType, DependenceNode<ValueType>,
                               DependenceGraph<ValueType>,
                               typename DependenceNode<ValueType>::iterator> {
  typedef icsa::DependenceNode<ValueType> NodeType;
  typedef typename icsa::DependenceNode<ValueType>::iterator ChildIteratorType;
};

template <typename ValueType>
struct ConstDGGraphTraits
    : public DGBaseGraphTraits<
          ValueType, const DependenceNode<ValueType>,
          const DependenceGraph<ValueType>,
          typename DependenceNode<ValueType>::const_iterator> {
  typedef const icsa::DependenceNode<ValueType> NodeType;
  typedef typename icsa::DependenceNode<ValueType>::const_iterator
      ChildIteratorType;
};
}
