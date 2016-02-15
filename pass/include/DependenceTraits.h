#ifndef ICSA_DSWP_DEPENDENCE_TRAITS_H
#define ICSA_DSWP_DEPENDENCE_TRAITS_H

#include "Dependence.h"
using icsa::DependenceGraph;

#include <string>
using std::string;

#include <map>
using std::map;
#include <set>
using std::set;

#include <memory>
using std::unique_ptr;

#include "llvm/ADT/GraphTraits.h"
using llvm::GraphTraits;

#include "llvm/Support/raw_ostream.h"
using llvm::errs;
using llvm::raw_fd_ostream;
using llvm::raw_ostream;
#include "llvm/Support/FileSystem.h"
using llvm::sys::fs::F_Text;
#include <system_error>
using std::error_code;

#include "llvm/Support/GraphWriter.h"
using llvm::WriteGraph;

#include "llvm/ADT/GraphTraits.h"

// Forward declarations.
namespace icsa {
template <typename ValueType> class DepGraphTraitsWrapper;
template <typename ValueType> class DepNodeTraitsWrapper;
}

/*
 * nodes-iterator:
 *  - constructed from 'graph-type'
 *  - dereferenced to 'node-type'
 *
 * child-iterator:
 *  - constructed from 'node-type'
 *  - dereferenced to 'node-type'
 */
namespace llvm {
using icsa::DepGraphTraitsWrapper;
using icsa::DepNodeTraitsWrapper;

template <typename ValueType>
class GraphTraits<DepGraphTraitsWrapper<ValueType>> {
public:
  typedef DepGraphTraitsWrapper<ValueType> GraphType;
  typedef DepNodeTraitsWrapper<ValueType> NodeType;

  typedef typename GraphType::nodes_iterator nodes_iterator;
  static nodes_iterator nodes_begin(const GraphType &G) {
    return G.nodes_begin();
  }
  static nodes_iterator nodes_end(const GraphType &G) { return G.nodes_end(); }

  typedef typename NodeType::child_iterator ChildIteratorType;
  static ChildIteratorType child_begin(NodeType *N) { return N->child_begin(); }
  static ChildIteratorType child_end(NodeType *N) { return N->child_end(); }
};
}

namespace icsa {

template <typename ValueType> class DepNodeIterator;
template <typename ValueType> class DepNodeChildIterator;

template <typename ValueType> class DepGraphTraitsWrapper {
public:
  typedef DepNodeTraitsWrapper<ValueType> NodeType;
  typedef DepNodeIterator<ValueType> nodes_iterator;

protected:
  map<const ValueType *, unique_ptr<NodeType>> Nodes;

public:
  DepGraphTraitsWrapper(const DependenceGraph<ValueType> &Graph) {
    for (auto I = Graph.nodes_cbegin(), E = Graph.nodes_cend(); I != E; ++I) {
      Nodes[I->first] =
          unique_ptr<NodeType>(new NodeType(*this, I->first, &I->second));
    }
  }

  nodes_iterator nodes_begin() const { return nodes_iterator(Nodes.begin()); }
  nodes_iterator nodes_end() const { return nodes_iterator(Nodes.end()); }

  NodeType *operator[](const ValueType *Value) { return Nodes[Value].get(); }

  void writeToFile(string Filename) {
    errs() << "Writing '" << Filename << "'...";

    error_code EC;
    raw_fd_ostream File(Filename, EC, F_Text);

    if (!EC) {
      WriteGraph(File, *this);
    } else {
      errs() << "  error opening file for writing!";
    }
    errs() << "\n";
  }
};

template <typename ValueType> class DepNodeTraitsWrapper {
public:
  typedef DepNodeChildIterator<ValueType> child_iterator;

protected:
  DepGraphTraitsWrapper<ValueType> &G;
  const ValueType *TheValue;
  const set<const ValueType *> *Children;

public:
  DepNodeTraitsWrapper(DepGraphTraitsWrapper<ValueType> &Graph,
                       const ValueType *Value, const set<const ValueType *> *C)
      : G(Graph), TheValue(Value), Children(C) {}

  const ValueType *getValue() const { return TheValue; }

  child_iterator child_begin() { return child_iterator(G, Children->begin()); }
  child_iterator child_end() { return child_iterator(G, Children->end()); }
};

// Iterator for all Dependence nodes.
template <typename ValueType>
class DepNodeIterator
    : public std::iterator<std::bidirectional_iterator_tag,
                           DepNodeTraitsWrapper<ValueType> *, int,
                           DepNodeTraitsWrapper<ValueType> *,
                           DepNodeTraitsWrapper<ValueType> *> {
  typedef std::iterator<std::bidirectional_iterator_tag,
                        DepNodeTraitsWrapper<ValueType> *, int,
                        DepNodeTraitsWrapper<ValueType> *,
                        DepNodeTraitsWrapper<ValueType> *> super;

public:
  typedef typename super::pointer pointer;
  typedef typename super::reference reference;

  typedef DepNodeTraitsWrapper<ValueType> NodeType;

private:
  typedef typename map<const ValueType *, unique_ptr<NodeType>>::const_iterator
      IndexType;
  IndexType idx;

public:
  // Begin iterator.
  explicit inline DepNodeIterator(IndexType Index) : idx(Index) {}

  inline bool operator==(const DepNodeIterator &x) const {
    return idx == x.idx;
  }
  inline bool operator!=(const DepNodeIterator &x) const {
    return !operator==(x);
  }

  inline reference operator*() const { return idx->second.get(); }
  inline pointer operator->() const { return operator*(); }

  inline DepNodeIterator &operator++() {
    ++idx;
    return *this;
  }

  inline DepNodeIterator operator++(int) {
    DepNodeIterator tmp = *this;
    ++*this;
    return tmp;
  }

  inline DepNodeIterator &operator--() {
    --idx;
    return *this;
  }
  inline DepNodeIterator operator--(int) {
    DepNodeIterator tmp = *this;
    --*this;
    return tmp;
  }
};

// Iterator for all Dependence nodes.
template <typename ValueType>
class DepNodeChildIterator
    : public std::iterator<std::bidirectional_iterator_tag,
                           DepNodeTraitsWrapper<ValueType> *, int,
                           DepNodeTraitsWrapper<ValueType> *,
                           DepNodeTraitsWrapper<ValueType> *> {
  typedef std::iterator<std::bidirectional_iterator_tag,
                        DepNodeTraitsWrapper<ValueType> *, int,
                        DepNodeTraitsWrapper<ValueType> *,
                        DepNodeTraitsWrapper<ValueType> *> super;

public:
  typedef typename super::pointer pointer;
  typedef typename super::reference reference;

  typedef DepGraphTraitsWrapper<ValueType> GraphType;
  typedef DepNodeTraitsWrapper<ValueType> NodeType;

private:
  typedef typename set<const ValueType *>::iterator IndexType;
  GraphType &G;
  IndexType idx;

public:
  // Begin iterator.
  explicit inline DepNodeChildIterator(GraphType &Graph, const IndexType &Index)
      : G(Graph), idx(Index) {}

  inline bool operator==(const DepNodeChildIterator &x) const {
    return idx == x.idx;
  }
  inline bool operator!=(const DepNodeChildIterator &x) const {
    return !operator==(x);
  }

  inline reference operator*() const { return G[*idx]; }
  inline pointer operator->() const { return operator*(); }

  inline DepNodeChildIterator &operator++() {
    ++idx;
    return *this;
  }

  inline DepNodeChildIterator operator++(int) {
    DepNodeChildIterator tmp = *this;
    ++*this;
    return tmp;
  }

  inline DepNodeChildIterator &operator--() {
    --idx;
    return *this;
  }
  inline DepNodeChildIterator operator--(int) {
    DepNodeChildIterator tmp = *this;
    --*this;
    return tmp;
  }
};
}

#endif
