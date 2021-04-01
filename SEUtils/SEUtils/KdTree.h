/** Copyright © 2021 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef _SEUTILS_KDTREE_H_
#define _SEUTILS_KDTREE_H_

#include <vector>
#include <memory>
#include <algorithm>

namespace SourceXtractor {

template <typename T>
struct KdTreeTraits {
  static double getCoord(const T& t, size_t index);
};

/**
 * @class KdTree
 * @brief A simple N-dimensional KdTree for speeding-up elements within range types of queries.
 *
 * template arguments: T type, a traits implemetation to access coordinates must be provided
 *                     N number of dimensions
 *                     S maximum number of elements in leaf nodes (must be >= 4, in practice we want larger anyway)
 */

template<typename T, size_t N=2, size_t S=100>
class KdTree {
public:
  using Traits = KdTreeTraits<T>;

  struct Coord {
    double coord[N];
  };

  explicit KdTree(const std::vector<T>& data);
  std::vector<T> findPointsWithinRadius(Coord coord, double radius) const;

private:
  class Node;
  class Leaf;
  class Split;

  std::shared_ptr<Node> m_root;
};

}

#include "_impl/KdTree.icpp"

#endif /* _SEUTILS_KDTREE_H_ */
