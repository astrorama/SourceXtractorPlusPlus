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
 * @file tests/src/ASDF/AsdfReader_test.cpp
 * @date 10/06/25
 * @author embray
 */

#include <boost/test/unit_test.hpp>
#include <fstream>

#include <ElementsKernel/Auxiliary.h>
#include <ElementsKernel/Exception.h>
#include <ElementsKernel/Temporary.h>

#include "SEFramework/ASDF/AsdfImageSource.h"
#include "SEFramework/ASDF/AsdfReader.h"
#include "SEFramework/Image/ImageFileReader.h"

#include "1px.asdf.h"

using namespace SourceXtractor;

struct AsdfReaderFixture {
  Elements::TempFile m_tmp_asdf;

  AsdfReaderFixture() {
    std::ofstream out{m_tmp_asdf.path().c_str()};
    out.write(reinterpret_cast<const char*>(image_asdf), image_asdf_len);
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (AsdfReader_test)


//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( read_file, AsdfReaderFixture ) {
  auto img = AsdfReader::readImage<SeFloat>(m_tmp_asdf.path().native());
  BOOST_CHECK_EQUAL(img->getWidth(), 1);
  BOOST_CHECK_EQUAL(img->getHeight(), 1);
  BOOST_CHECK_EQUAL(img->getChunk(0, 0, 1, 1)->getValue(0, 0), 42);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE ( image_source, AsdfReaderFixture ) {
  auto img = AsdfImageSource(m_tmp_asdf.path().native());
  BOOST_CHECK_EQUAL(img.getNdim(), 2);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE ( detect_file_type ) {
  auto reader = ImageFileReader::create(Elements::getAuxiliaryPath("with_primary.asdf").native());
  auto* asdf_reader = dynamic_cast<AsdfReader*>(reader.get());
  BOOST_CHECK(asdf_reader != nullptr);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE ( iterate ) {
  auto reader = ImageFileReader::create(Elements::getAuxiliaryPath("with_primary.asdf").native());
  // Should just return one image, from the primary HDU (the next HDU in this file is a table)
  int image_count = 0;
  for (const auto& img_source: *reader) {
    auto asdf_source = std::dynamic_pointer_cast<AsdfImageSource>(img_source);
    BOOST_CHECK(asdf_source != nullptr);
    image_count++;
  }
  BOOST_CHECK_EQUAL(image_count, 1);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE ( open_with_extname ) {
  auto reader = ImageFileReader::create(Elements::getAuxiliaryPath("multiple_hdu.asdf").native());
  auto* asdf_reader = dynamic_cast<AsdfReader*>(reader.get());
  BOOST_CHECK(asdf_reader != nullptr);
  auto img = reader->get("IMAGE2");
  BOOST_CHECK_EQUAL(img->getWidth(), 1);
  BOOST_CHECK_EQUAL(img->getHeight(), 1);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( missing_file ) {
  BOOST_CHECK_THROW(AsdfReader::readImage<SeFloat>("/not/existing/path"), Elements::Exception);
}

//-----------------------------------------------------------------------------


BOOST_AUTO_TEST_SUITE_END ()
