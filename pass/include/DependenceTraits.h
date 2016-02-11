#ifndef ICSA_DSWP_DEPENDENCE_TRAITS_H
#define ICSA_DSWP_DEPENDENCE_TRAITS_H

#include "Dependence.h"
using icsa::DependenceGraph;

#include <map>
using std::map;
#include <set>
using std::set;

#include <memory>
using std::unique_ptr;


#include <string>
using std::string;

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

namespace icsa {

/* child-iterator:
 *  - dereferenced to 'node-type'
 *  - constructed from 'node-type'
 *
 * nodes-iterator:
 *  - dereferenced to 'node-type'
 *  - constructed from 'graph-type'
 */

template <typename VT> class NparasiteChildIterator;
template <typename VT> class DepGraphTraitsWrapper;

template <typename VT> class DepNodeTraitsWrapper {
public:
  typedef NparasiteChildIterator<VT> child_iterator;

protected:
  DepGraphTraitsWrapper<VT> &G;
  const VT *TheValue;
  const set<VT *> *Children;

public:
  DepNodeTraitsWrapper(DepGraphTraitsWrapper<VT> &Graph, const VT *Value, const set<VT *> *C)
      : G(Graph), TheValue(Value), Children(C) {}

  const VT *getValue() const { return TheValue; }

  child_iterator child_begin() { return child_iterator(G, Children->begin()); }
  child_iterator child_end() { return child_iterator(G, Children->end()); }
};

// Iterator for all Dependence nodes.
template <typename VT>
class NparasiteIterator
    : public std::iterator<std::bidirectional_iterator_tag, DepNodeTraitsWrapper<VT> *, int,
                           DepNodeTraitsWrapper<VT> *, DepNodeTraitsWrapper<VT> *> {
  typedef std::iterator<std::bidirectional_iterator_tag, DepNodeTraitsWrapper<VT> *, int,
                        DepNodeTraitsWrapper<VT> *, DepNodeTraitsWrapper<VT> *> super;

public:
  typedef typename super::pointer pointer;
  typedef typename super::reference reference;

  typedef DepNodeTraitsWrapper<VT> NodeType;

private:
  typedef typename map<VT *, unique_ptr<NodeType>>::const_iterator IndexType;
  IndexType idx;

public:
  // Begin iterator.
  explicit inline NparasiteIterator(IndexType Index) : idx(Index) {}

  inline bool operator==(const NparasiteIterator &x) const {
    return idx == x.idx;
  }
  inline bool operator!=(const NparasiteIterator &x) const {
    return !operator==(x);
  }

  inline reference operator*() const { return idx->second.get(); }
  inline pointer operator->() const { return operator*(); }

  inline NparasiteIterator &operator++() {
    ++idx;
    return *this;
  }

  inline NparasiteIterator operator++(int) {
    NparasiteIterator tmp = *this;
    ++*this;
    return tmp;
  }

  inline NparasiteIterator &operator--() {
    --idx;
    return *this;
  }
  inline NparasiteIterator operator--(int) {
    NparasiteIterator tmp = *this;
    --*this;
    return tmp;
  }
};


template <typename VT> class DepGraphTraitsWrapper {
public:
  typedef DepNodeTraitsWrapper<VT> NodeType;
  typedef NparasiteIterator<VT> nodes_iterator;

protected:
  map<VT *, unique_ptr<NodeType>> Nodes;

public:
  DepGraphTraitsWrapper(const DependenceGraph<VT> &Graph) {
    for (auto I = Graph.cbegin(), E = Graph.cend(); I != E; ++I) {
      Nodes[I->first] =
          unique_ptr<NodeType>(new NodeType(*this, I->first, &I->second));
    }
  }

  nodes_iterator nodes_begin() const { return nodes_iterator(Nodes.begin()); }
  nodes_iterator nodes_end() const { return nodes_iterator(Nodes.end()); }

  NodeType *operator[](VT *Value) { return Nodes[Value].get(); }

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


// Iterator for all Dependence nodes.
template <typename VT>
class NparasiteChildIterator
    : public std::iterator<std::bidirectional_iterator_tag, DepNodeTraitsWrapper<VT> *, int,
                           DepNodeTraitsWrapper<VT> *, DepNodeTraitsWrapper<VT> *> {
  typedef std::iterator<std::bidirectional_iterator_tag, DepNodeTraitsWrapper<VT> *, int,
                        DepNodeTraitsWrapper<VT> *, DepNodeTraitsWrapper<VT> *> super;

public:
  typedef typename super::pointer pointer;
  typedef typename super::reference reference;

  typedef DepGraphTraitsWrapper<VT> GraphType;
  typedef DepNodeTraitsWrapper<VT> NodeType;

private:
  typedef typename set<VT *>::iterator IndexType;
  GraphType &G;
  IndexType idx;

public:
  // Begin iterator.
  explicit inline NparasiteChildIterator(GraphType &Graph,
                                         const IndexType &Index)
      : G(Graph), idx(Index) {}

  inline bool operator==(const NparasiteChildIterator &x) const {
    return idx == x.idx;
  }
  inline bool operator!=(const NparasiteChildIterator &x) const {
    return !operator==(x);
  }

  inline reference operator*() const { return G[*idx]; }
  inline pointer operator->() const { return operator*(); }

  inline NparasiteChildIterator &operator++() {
    ++idx;
    return *this;
  }

  inline NparasiteChildIterator operator++(int) {
    NparasiteChildIterator tmp = *this;
    ++*this;
    return tmp;
  }

  inline NparasiteChildIterator &operator--() {
    --idx;
    return *this;
  }
  inline NparasiteChildIterator operator--(int) {
    NparasiteChildIterator tmp = *this;
    --*this;
    return tmp;
  }
};
}

#include "llvm/ADT/GraphTraits.h"

namespace llvm {
using icsa::DepGraphTraitsWrapper;
using icsa::DepNodeTraitsWrapper;

template <typename VT> class GraphTraits<DepGraphTraitsWrapper<VT>> {
public:
  typedef DepGraphTraitsWrapper<VT> GraphType;
  typedef DepNodeTraitsWrapper<VT> NodeType;

  typedef typename GraphType::nodes_iterator nodes_iterator;
  static nodes_iterator nodes_begin(const GraphType &G) { return G.nodes_begin(); }
  static nodes_iterator nodes_end(const GraphType &G) { return G.nodes_end(); }

  typedef typename NodeType::child_iterator ChildIteratorType;
  static ChildIteratorType child_begin(NodeType *N) { return N->child_begin(); }
  static ChildIteratorType child_end(NodeType *N) { return N->child_end(); }
};

}

#endif
