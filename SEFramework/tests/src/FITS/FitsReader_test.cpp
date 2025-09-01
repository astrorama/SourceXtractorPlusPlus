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
#include <fstream>

#include <ElementsKernel/Auxiliary.h>
#include <ElementsKernel/Exception.h>
#include <ElementsKernel/Temporary.h>

#include "SEFramework/FITS/FitsReader.h"
#include "SEFramework/Image/ImageFileReader.h"

#include "1px.fits.h"

using namespace SourceXtractor;

struct FitsReaderFixture {
  Elements::TempFile m_tmp_fits;

  FitsReaderFixture() {
    std::ofstream out{m_tmp_fits.path().c_str()};
    out.write(reinterpret_cast<const char*>(image_fits), image_fits_len);
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (FitsReader_test)


//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( read_file, FitsReaderFixture ) {
  auto img = FitsReader::readFile<SeFloat>(m_tmp_fits.path().native());
  BOOST_CHECK_EQUAL(img->getWidth(), 1);
  BOOST_CHECK_EQUAL(img->getHeight(), 1);
  BOOST_CHECK_EQUAL(img->getChunk(0, 0, 1, 1)->getValue(0, 0), 42);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE ( image_source, FitsReaderFixture ) {
  auto img = FitsImageSource(m_tmp_fits.path().native());
  int naxis;
  img.readFitsKeyword("NAXIS", naxis);
  BOOST_CHECK_EQUAL(naxis, 2);
}


BOOST_AUTO_TEST_CASE ( detect_file_type ) {
  auto reader = ImageFileReader::create(Elements::getAuxiliaryPath("with_primary.fits").native());
  auto* fits_reader = dynamic_cast<FitsReader*>(reader.get());
  BOOST_CHECK(fits_reader != nullptr);
}


BOOST_AUTO_TEST_CASE ( open_with_extname ) {
  auto reader = ImageFileReader::create(
    Elements::getAuxiliaryPath("multiple_hdu.fits").native() + "[IMAGE2]");
  auto* fits_reader = dynamic_cast<FitsReader*>(reader.get());
  BOOST_CHECK(fits_reader != nullptr);
  auto img = reader->get();
  BOOST_CHECK_EQUAL(img->getWidth(), 1);
  BOOST_CHECK_EQUAL(img->getHeight(), 1);
}


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( missing_file ) {
  BOOST_CHECK_THROW(FitsReader::readFile<SeFloat>("/not/existing/path"), Elements::Exception);
}

//-----------------------------------------------------------------------------


BOOST_AUTO_TEST_SUITE_END ()


