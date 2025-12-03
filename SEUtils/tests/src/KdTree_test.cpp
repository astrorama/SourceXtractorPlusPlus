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

#include <boost/test/unit_test.hpp>
#include <cmath>

#include "SEUtils/KdTree.h"

namespace SourceXtractor {

struct TestPoint {
  double m_x, m_y;
  int m_id;  // For identification in tests
  
  TestPoint(double x, double y, int id = 0) : m_x(x), m_y(y), m_id(id) {}
};

bool operator==(const TestPoint& a, const TestPoint& b) {
  return a.m_x == b.m_x && a.m_y == b.m_y && a.m_id == b.m_id;
}

template <>
struct KdTreeTraits<TestPoint> {
  static double getCoord(const TestPoint& t, size_t index) {
    if (index == 0) {
      return t.m_x;
    } else {
      return t.m_y;
    }
  }
};

// Test point for 3D tests
struct TestPoint3D {
  double m_x, m_y, m_z;
  int m_id;
  
  TestPoint3D(double x, double y, double z, int id = 0) : m_x(x), m_y(y), m_z(z), m_id(id) {}
};

bool operator==(const TestPoint3D& a, const TestPoint3D& b) {
  return a.m_x == b.m_x && a.m_y == b.m_y && a.m_z == b.m_z && a.m_id == b.m_id;
}

template <>
struct KdTreeTraits<TestPoint3D> {
  static double getCoord(const TestPoint3D& t, size_t index) {
    if (index == 0) {
      return t.m_x;
    } else if (index == 1) {
      return t.m_y;
    } else {
      return t.m_z;
    }
  }
};

BOOST_AUTO_TEST_SUITE (KdTree_test)

BOOST_AUTO_TEST_CASE( smoke_test ) {
  std::vector<TestPoint> data = {{1.0, 2.0, 1}, {3.0, 4.0, 2}, {5.0, 6.0, 3}};
  KdTree<TestPoint> tree(data);
}

BOOST_AUTO_TEST_CASE( empty_tree_test ) {
  std::vector<TestPoint> data = {};
  // This should not crash - testing edge case
  BOOST_CHECK_NO_THROW(KdTree<TestPoint> tree(data));
}

BOOST_AUTO_TEST_CASE( single_point_test ) {
  std::vector<TestPoint> data = {{1.0, 2.0, 1}};
  KdTree<TestPoint> tree(data);
  
  KdTree<TestPoint>::Coord coord;
  coord.coord[0] = 1.0;
  coord.coord[1] = 2.0;
  
  auto result = tree.findPointsWithinRadius(coord, 0.1);
  BOOST_CHECK_EQUAL(result.size(), 1);
  BOOST_CHECK(result[0] == TestPoint(1.0, 2.0, 1));
  
  auto nearest = tree.findNearest(coord);
  BOOST_CHECK(nearest == TestPoint(1.0, 2.0, 1));
}

BOOST_AUTO_TEST_CASE( simple_radius_search_test ) {
  std::vector<TestPoint> data = {
    {1.0, 1.0, 1}, {2.0, 2.0, 2}, {3.0, 3.0, 3}, {10.0, 10.0, 4}
  };
  KdTree<TestPoint> tree(data);
  
  KdTree<TestPoint>::Coord coord;
  coord.coord[0] = 2.0;
  coord.coord[1] = 2.0;
  
  // Search with radius 2.0 - should find points at (1,1), (2,2), (3,3)
  auto result = tree.findPointsWithinRadius(coord, 2.0);
  BOOST_CHECK_EQUAL(result.size(), 3);
  
  // Search with radius 1.0 - should find only point at (2,2)
  result = tree.findPointsWithinRadius(coord, 1.0);
  BOOST_CHECK_EQUAL(result.size(), 1);
  BOOST_CHECK(result[0] == TestPoint(2.0, 2.0, 2));
  
  // Search with large radius - should find all points
  result = tree.findPointsWithinRadius(coord, 15.0);
  BOOST_CHECK_EQUAL(result.size(), 4);
}

BOOST_AUTO_TEST_CASE( nearest_neighbor_test ) {
  std::vector<TestPoint> data = {
    {1.0, 1.0, 1}, {2.0, 2.0, 2}, {3.0, 3.0, 3}, {10.0, 10.0, 4}
  };
  KdTree<TestPoint> tree(data);
  
  KdTree<TestPoint>::Coord coord;
  coord.coord[0] = 2.1;
  coord.coord[1] = 1.9;
  
  auto nearest = tree.findNearest(coord);
  BOOST_CHECK(nearest == TestPoint(2.0, 2.0, 2));
  
  // Test corner case - equidistant points
  coord.coord[0] = 1.5;
  coord.coord[1] = 1.5;
  nearest = tree.findNearest(coord);
  // Should return one of the two closest points (1,1) or (2,2)
  BOOST_CHECK(nearest == TestPoint(1.0, 1.0, 1) || nearest == TestPoint(2.0, 2.0, 2));
}

BOOST_AUTO_TEST_CASE( large_dataset_test ) {
  std::vector<TestPoint> data;
  
  // Create a grid of 1000 points
  for (int i = 0; i < 1000; i++) {
    double x = 5.0 + (i % 100) / 10.0;
    double y = 2.0 + (i / 100) / 5.0;
    data.emplace_back(x, y, i);
  }
  
  KdTree<TestPoint> tree(data);
  
  KdTree<TestPoint>::Coord coord;
  coord.coord[0] = 7.0;
  coord.coord[1] = 3.0;
  
  // Test radius search on large dataset
  auto result = tree.findPointsWithinRadius(coord, 1.0);
  BOOST_CHECK_GT(result.size(), 0);
  
  // Test nearest neighbor on large dataset
  auto nearest = tree.findNearest(coord);
  BOOST_CHECK_GT(nearest.m_id, -1);  // Should find a valid point
}

BOOST_AUTO_TEST_CASE( three_dimensional_test ) {
  std::vector<TestPoint3D> data = {
    {1.0, 1.0, 1.0, 1}, {2.0, 2.0, 2.0, 2}, {3.0, 3.0, 3.0, 3}, {10.0, 10.0, 10.0, 4}
  };
  KdTree<TestPoint3D, 3> tree(data);
  
  KdTree<TestPoint3D, 3>::Coord coord;
  coord.coord[0] = 2.0;
  coord.coord[1] = 2.0;
  coord.coord[2] = 2.0;
  
  // Test radius search in 3D
  auto result = tree.findPointsWithinRadius(coord, 2.0);
  BOOST_CHECK_EQUAL(result.size(), 3);  // Should find first 3 points
  
  // Test nearest neighbor in 3D
  coord.coord[0] = 2.1;
  coord.coord[1] = 1.9;
  coord.coord[2] = 2.1;
  
  auto nearest = tree.findNearest(coord);
  BOOST_CHECK(nearest == TestPoint3D(2.0, 2.0, 2.0, 2));
}

BOOST_AUTO_TEST_CASE( small_leaf_size_test ) {
  std::vector<TestPoint> data = {
    {1.0, 1.0, 1}, {2.0, 2.0, 2}, {3.0, 3.0, 3}, {4.0, 4.0, 4}, {5.0, 5.0, 5}
  };
  
  // Use small leaf size to force tree structure
  KdTree<TestPoint, 2, 2> tree(data);
  
  KdTree<TestPoint, 2, 2>::Coord coord;
  coord.coord[0] = 3.0;
  coord.coord[1] = 3.0;
  
  auto result = tree.findPointsWithinRadius(coord, 1.5);
  BOOST_CHECK_GE(result.size(), 1);
  
  auto nearest = tree.findNearest(coord);
  BOOST_CHECK(nearest == TestPoint(3.0, 3.0, 3));
}

BOOST_AUTO_TEST_CASE( edge_case_identical_coordinates ) {
  std::vector<TestPoint> data = {
    {1.0, 1.0, 1}, {1.0, 1.0, 2}, {1.0, 1.0, 3}, {2.0, 2.0, 4}
  };
  KdTree<TestPoint> tree(data);
  
  KdTree<TestPoint>::Coord coord;
  coord.coord[0] = 1.0;
  coord.coord[1] = 1.0;
  
  auto result = tree.findPointsWithinRadius(coord, 0.1);
  BOOST_CHECK_EQUAL(result.size(), 3);  // Should find all three identical points
  
  auto nearest = tree.findNearest(coord);
  // Should return one of the three identical points
  BOOST_CHECK(nearest.m_x == 1.0 && nearest.m_y == 1.0);
}

BOOST_AUTO_TEST_CASE( stress_test_performance ) {
  std::vector<TestPoint> data;
  
  // Create 10000 random-ish points
  for (int i = 0; i < 10000; i++) {
    double x = (i * 17) % 1000 / 10.0;  // Pseudo-random distribution
    double y = (i * 23) % 1000 / 10.0;
    data.emplace_back(x, y, i);
  }
  
  KdTree<TestPoint> tree(data);
  
  KdTree<TestPoint>::Coord coord;
  coord.coord[0] = 50.0;
  coord.coord[1] = 50.0;
  
  // This should complete reasonably quickly due to tree structure
  auto result = tree.findPointsWithinRadius(coord, 10.0);
  BOOST_CHECK_GE(result.size(), 0);
  
  auto nearest = tree.findNearest(coord);
  BOOST_CHECK_GE(nearest.m_id, 0);
}

BOOST_AUTO_TEST_SUITE_END ()

}
