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

template<typename T, size_t N=2, size_t S=100>
class KdTree {
public:
  using Traits = KdTreeTraits<T>;

  struct Coord {
    double coord[N];
  };

  explicit KdTree(const std::vector<T>& data) : m_root(new Leaf(data)) {
    if (data.size() > S) {
      m_root = std::make_shared<Split>(data, 0);
    } else {
      m_root = std::make_shared<Leaf>(data);
    }
  }

  std::vector<T> findPointsWithinRadius(Coord coord, double radius) const {
    return m_root->findPointsWithinRadius(coord, radius);
  }

private:
  class Node {
  public:
    virtual std::vector<T> findPointsWithinRadius(Coord coord, double radius) const = 0;
    virtual ~Node() = default;
  };

  class Leaf : public Node {
  public:
    explicit Leaf(const std::vector<T>& data) : m_data(data) {}
    virtual ~Leaf() = default;

    virtual std::vector<T> findPointsWithinRadius(Coord coord, double radius) const {
      std::vector<T> selection;
      for (auto& entry : m_data) {
        double square_dist = 0.0;
        for (size_t i =0; i < N; i++) {
          double delta = Traits::getCoord(entry, i) - coord.coord[i];
          square_dist += delta * delta;
        }
        if (square_dist < radius*radius) {
          selection.push_back(entry);
        }
      }
      return selection;
    }

  private:
    const std::vector<T> m_data;
  };

  class Split : public Node  {
  public:
    virtual ~Split() = default;
    explicit Split(std::vector<T> data, size_t axis) : m_axis(axis) {
      std::sort(data.begin(), data.end(), [axis](const T& a, const T& b) -> bool {
        return Traits::getCoord(a, axis) < Traits::getCoord(b, axis);
      });

      double a = Traits::getCoord(data.at(data.size() / 2 - 1), axis);
      double b = Traits::getCoord(data.at(data.size() / 2), axis);

      if (a == b) {
        // avoid a possible rounding issue
        m_split_value = a;
      } else {
        m_split_value = (a + b) / 2.0;
      }

      std::vector<T> left(data.begin(), data.begin() + data.size() / 2);
      std::vector<T> right(data.begin() + data.size() / 2, data.end());

      if (left.size() > S) {
        m_left_child = std::make_shared<Split>(left, (axis+1) % N);
      } else {
        m_left_child = std::make_shared<Leaf>(left);
      }
      if (right.size() > S) {
        m_right_child = std::make_shared<Split>(right, (axis+1) % N);
      } else {
        m_right_child = std::make_shared<Leaf>(right);
      }
    }

    virtual std::vector<T> findPointsWithinRadius(Coord coord, double radius) const {
      if (coord.coord[m_axis] + radius < m_split_value) {
        return m_left_child->findPointsWithinRadius(coord, radius);
      } else if (coord.coord[m_axis] - radius > m_split_value) {
        return m_right_child->findPointsWithinRadius(coord, radius);
      } else {
        auto left = m_left_child->findPointsWithinRadius(coord, radius);
        auto right = m_right_child->findPointsWithinRadius(coord, radius);

        std::vector<T> merge;
        merge.reserve(left.size() + right.size());
        merge.insert(merge.end(), left.begin(), left.end());
        merge.insert(merge.end(), right.begin(), right.end());

        return merge;
      }
    }

  private:
    size_t m_axis;
    double m_split_value;

    std::shared_ptr<Node> m_left_child;
    std::shared_ptr<Node> m_right_child;
  };

  std::shared_ptr<Node> m_root;
};

}

#endif /* _SEUTILS_KDTREE_H_ */
