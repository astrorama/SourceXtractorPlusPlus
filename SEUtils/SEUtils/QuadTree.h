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

namespace SourceXtractor {

template <typename T>
struct QuadTreeTraits {
  static double getCoord(const T& t, size_t index);
};

template<typename T>
class QuadTree {
public:
  using Traits = QuadTreeTraits<T>;

  struct Coord {
    double x, y;
  };

  QuadTree(size_t capacity=100);
  QuadTree(const QuadTree& tree);

  void add(const T& data);
  void remove(const T& data);
  std::vector<T> getPointsWithinRange(Coord c, double range) const;

private:
  void addLocally(const T& data);
  void split();
  void expand(Coord c);
  size_t getQuadrant(Coord c) const;
  bool isContained(Coord c) const;

  Coord getQuadrantMin(size_t quadrant) const;
  Coord getQuadrantMax(size_t quadrant) const;

  size_t m_capacity;

  bool m_is_divided;
  Coord m_min, m_max;
  std::vector<T> m_data;
  std::shared_ptr<QuadTree<T>> m_sub_trees[4];
};

}

#include "_impl/QuadTree.icpp"

#endif /* _SEUTILS_QUADTREE_H_ */
