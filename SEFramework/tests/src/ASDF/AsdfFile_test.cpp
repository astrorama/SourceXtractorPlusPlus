/** Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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
/**
 * @file tests/src/AsdfFile_test.cpp
 * @date 08/14/25
 * @author embray
 */

#include <boost/test/unit_test.hpp>

#include <ElementsKernel/Auxiliary.h>
#include <ElementsKernel/Exception.h>

#include "SEFramework/ASDF/AsdfFile.h"

using namespace SourceXtractor;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (AsdfFile_test)


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( open_file ) {
  BOOST_CHECK_NO_THROW({
    AsdfFile asdf_file(Elements::getAuxiliaryPath("basic.asdf").native());
  });
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( missing_file ) {
  BOOST_CHECK_THROW({
    AsdfFile asdf_file("/does/not/exist");
  }, Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( get_ndarray_by_index ) {
  AsdfFile asdf_file(Elements::getAuxiliaryPath("basic.asdf").native());
  auto ndarray = asdf_file.getNdarray(0);
  BOOST_CHECK_EQUAL(ndarray->ndim(), 1);
  auto shape = ndarray->shape();
  std::vector<uint64_t> expected_shape{8};
  BOOST_CHECK_EQUAL_COLLECTIONS(
    shape.begin(), shape.end(),
    expected_shape.begin(), expected_shape.end()
  );
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( get_ndarray_by_path ) {
  AsdfFile asdf_file(Elements::getAuxiliaryPath("basic.asdf").native());
  auto ndarray = asdf_file.getNdarray("data");
  BOOST_CHECK_EQUAL(ndarray->ndim(), 1);
  auto shape = ndarray->shape();
  std::vector<uint64_t> expected_shape{8};
  BOOST_CHECK_EQUAL_COLLECTIONS(
    shape.begin(), shape.end(),
    expected_shape.begin(), expected_shape.end()
  );
}

//-----------------------------------------------------------------------------


BOOST_AUTO_TEST_SUITE_END ()
