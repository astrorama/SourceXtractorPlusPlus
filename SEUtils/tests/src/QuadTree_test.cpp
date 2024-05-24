/** Copyright © 2019-2024 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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

#include "SEUtils/QuadTree.h"

namespace SourceXtractor {

struct Test {
  double m_x, m_y;
};

bool operator==(const Test&a, const Test&b) {
  return a.m_x == b.m_x && a.m_y == b.m_y;
};

template <>
struct QuadTreeTraits<Test> {
  static double getCoord(Test t, size_t index) {
    if (index == 0) {
      return t.m_x;
    } else {
      return t.m_y;
    }
  }
};


BOOST_AUTO_TEST_SUITE (QuadTree_test)

BOOST_AUTO_TEST_CASE( smoke_test ) {
  QuadTree<Test> tree(100);
}

BOOST_AUTO_TEST_CASE( add_test ) {
  QuadTree<Test> tree(100);

  tree.add({1,2});
  tree.add({3,4});
  tree.add({5,6});

  auto result = tree.getPointsWithinRange({0,0}, 100);
  BOOST_CHECK_EQUAL(result.size(), 3);

  for (int i = 0; i<1000; i++) {
    tree.add({5+i/1000.0,2+i/500.0});
  }

  // add some well outside the area
  for (int i = 0; i<1000; i++) {
    tree.add({500+i/1000.0,2+i/500.0});
  }

  result = tree.getPointsWithinRange({0,0}, 100);
  BOOST_CHECK_EQUAL(result.size(), 1003);
}

BOOST_AUTO_TEST_CASE( simple_remove_test ) {
  QuadTree<Test> tree(100);

  tree.add({1,2});
  tree.add({3,4});
  tree.add({5,6});

  tree.remove({3,4});

  auto result = tree.getPointsWithinRange({0,0}, 100);
  BOOST_CHECK_EQUAL(result.size(), 2);
}

BOOST_AUTO_TEST_CASE( remove_in_tree_test ) {
  QuadTree<Test> tree(100);

  for (int i = 0; i<1000; i++) {
    tree.add({5+i/1000.0,2+i/500.0});
  }

  tree.add({1,2});
  tree.add({3,4});
  tree.add({5,6});

  tree.remove({3,4});

  auto result = tree.getPointsWithinRange({0,0}, 100);
  BOOST_CHECK_EQUAL(result.size(), 1002);
}



BOOST_AUTO_TEST_SUITE_END ()

}
