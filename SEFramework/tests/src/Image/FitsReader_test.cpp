/**
 * @file tests/src/FitsReader_test.cpp
 * @date 06/14/16
 * @author nikoapos
 */

#include <boost/test/unit_test.hpp>
#include <fstream>

#include "SEFramework/Image/FitsReader.h"
#include "SEFramework/Filesystem/TemporaryFile.h"

#include "1px.fits.h"

using namespace SExtractor;

struct FitsReaderFixture {
  TemporaryFile m_tmp_fits;

  FitsReaderFixture(): m_tmp_fits{true} {
    std::ofstream out{m_tmp_fits.getPath().c_str()};
    out.write(reinterpret_cast<const char*>(image_fits), image_fits_len);
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (FitsReader_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( read_file, FitsReaderFixture ) {
  auto img = FitsReader<SeFloat>::readFile(m_tmp_fits.getPath());
  BOOST_CHECK_EQUAL(img->getWidth(), 1);
  BOOST_CHECK_EQUAL(img->getHeight(), 1);
  BOOST_CHECK_EQUAL(img->getValue(0, 0), 42);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( missing_file ) {
  BOOST_CHECK_THROW(FitsReader<SeFloat>::readFile("/not/existing/path"), Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


