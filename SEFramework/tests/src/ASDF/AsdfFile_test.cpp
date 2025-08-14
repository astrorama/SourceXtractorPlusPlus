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
 * @file tests/src/FitsReader_test.cpp
 * @date 06/14/16
 * @author nikoapos
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


BOOST_AUTO_TEST_SUITE_END ()
