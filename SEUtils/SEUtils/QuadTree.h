/*
 * QuadTree.h
 *
 *  Created on: Nov 28, 2023
 *      Author: mschefer
 */

#ifndef _SEUTILS_QUADTREE_H_
#define _SEUTILS_QUADTREE_H_


#include <vector>
#include <memory>
#include <algorithm>

namespace SourceXtractor {

template <typename T, size_t N = 100>
struct QuadTreeTraits;

template<typename T, size_t N = 100>
class QuadTree {
public:
  using Traits = QuadTreeTraits<T>;

  struct Coord {
    double x, y;
  };

  void add(T&& data);

private:
  class Node;
  class Branch;
  class Leaf;

  std::shared_ptr<Node> m_root;
};

template <typename T, size_t N>
struct QuadTreeTraits {
  static typename QuadTree<T, N>::Coord getCoord(const T& t);
};

}

#include "_impl/QuadTree.icpp"

#endif /* _SEUTILS_QUADTREE_H_ */
