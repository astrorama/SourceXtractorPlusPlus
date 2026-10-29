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
#include <FilePool/FileManager.h>

using namespace SourceXtractor;


struct AsdfFileFixture {
  std::string primary_path;
  std::string wcs_path;
  std::string metadata_path;

  /// AsdfFile::Metadata is built from a pooled file handle, as AsdfImageSource does
  AsdfFile::Metadata openMetadata(const std::string& path) {
    return AsdfFile::Metadata(
        Euclid::FilePool::FileManager::getDefault()->getFileHandler(path));
  }

  AsdfFileFixture() {
    primary_path = Elements::getAuxiliaryPath("with_primary.asdf").native();
    wcs_path = Elements::getAuxiliaryPath("wcs_header.asdf").native();
    metadata_path = Elements::getAuxiliaryPath("metadata.asdf").native();
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (AsdfFile_test)


//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( open_file, AsdfFileFixture ) {
  BOOST_CHECK_NO_THROW({
    AsdfFile asdf_file(primary_path);
  });
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( missing_file ) {
  BOOST_CHECK_THROW({
    AsdfFile asdf_file("/does/not/exist");
  }, Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( get_ndarray_by_index, AsdfFileFixture ) {
  AsdfFile asdf_file(primary_path);
  auto ndarray = asdf_file.getNdarray(0);
  BOOST_CHECK_EQUAL(ndarray->ndim(), 2);
  auto shape = ndarray->shape();
  std::vector<uint64_t> expected_shape{1, 1};
  BOOST_CHECK_EQUAL_COLLECTIONS(
    shape.begin(), shape.end(),
    expected_shape.begin(), expected_shape.end()
  );
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( get_ndarray_by_path, AsdfFileFixture ) {
  AsdfFile asdf_file(primary_path);
  auto ndarray = asdf_file.getNdarray("PRIMARY");
  BOOST_CHECK_EQUAL(ndarray->ndim(), 2);
  auto shape = ndarray->shape();
  std::vector<uint64_t> expected_shape{1, 1};
  BOOST_CHECK_EQUAL_COLLECTIONS(
    shape.begin(), shape.end(),
    expected_shape.begin(), expected_shape.end()
  );
  BOOST_CHECK_EQUAL(ndarray->getPath(), "/PRIMARY");
}


#ifdef WITH_ASDF_GWCS
BOOST_FIXTURE_TEST_CASE( get_gwcs_eval_auto, AsdfFileFixture ) {
  AsdfFile asdf_file(wcs_path);
  auto eval = asdf_file.getGwcsEval();

  BOOST_CHECK(eval != nullptr);
}


BOOST_FIXTURE_TEST_CASE( get_gwcs_eval_by_path, AsdfFileFixture ) {
  AsdfFile asdf_file(wcs_path);

  BOOST_CHECK(asdf_file.getGwcsEval("wcs1") != nullptr);
  BOOST_CHECK(asdf_file.getGwcsEval("wcs2") != nullptr);
  BOOST_CHECK_THROW(asdf_file.getGwcsEval("does-not-exist"), Elements::Exception);
}

#endif /* WITH_ASDF_GWCS */

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( metadata_get_types, AsdfFileFixture ) {
  auto metadata = openMetadata(metadata_path);

  // Each scalar keeps its own type rather than being stringified.  Integers are
  // the case worth pinning: asdf_value_as_bool() accepts a uint8, so dispatching
  // on which accessor succeeds would record 6 as `true`.
  BOOST_CHECK_EQUAL(*metadata.getAs<std::string>("/meta/instrument/optical_element"), "F158");
  BOOST_CHECK_EQUAL(boost::get<double>(metadata.get("/meta/exposure/exposure_time")->m_value), 107.5);
  BOOST_CHECK_EQUAL(boost::get<int64_t>(metadata.get("/meta/exposure/ngroups")->m_value), 6);
  BOOST_CHECK_EQUAL(boost::get<int64_t>(metadata.get("/meta/saturation")->m_value), 65535);
  BOOST_CHECK_EQUAL(boost::get<bool>(metadata.get("/meta/exposure/is_science")->m_value), true);

  // ASDF null has no MetadataEntry counterpart, so it reads empty
  BOOST_CHECK_EQUAL(boost::get<std::string>(metadata.get("/meta/empty")->m_value), "");

  // A leading slash is not required
  BOOST_CHECK_EQUAL(*metadata.getAs<double>("meta/gain"), 2.5);
}


BOOST_FIXTURE_TEST_CASE( metadata_get_absent, AsdfFileFixture ) {
  auto metadata = openMetadata(metadata_path);

  // Missing values just return std::nullopt
  BOOST_CHECK(metadata.get("/meta/nope") == std::nullopt);
  BOOST_CHECK(!metadata.contains("/meta/nope"));

  // A container has no value of its own for now; the metadata interface
  // only allows lookup of scalar values for the moment as a FITS legacy,
    // but that could be changed if the need ever arises.
  BOOST_CHECK(!metadata.contains("/meta"));
  BOOST_CHECK(!metadata.contains("/meta/instrument"));
  BOOST_CHECK(!metadata.contains("/meta/filters"));
  BOOST_CHECK(!metadata.contains("/data"));
}


BOOST_FIXTURE_TEST_CASE( metadata_for_each_walks_every_container, AsdfFileFixture ) {
  auto metadata = openMetadata(metadata_path);
  auto seen = metadata.toMap();

  // Sequence elements are addressed by index, in the YAML path convention
  // used by libasdf
  BOOST_CHECK_EQUAL(boost::get<std::string>(seen.at("/meta/filters/0").m_value), "F158");
  BOOST_CHECK_EQUAL(boost::get<std::string>(seen.at("/meta/filters/1").m_value), "F184");

  // ndarray nodes are mappings too and may carry user keys, so they are walked
  // like anything else.  The binary block is not a scalar, so it never appears.
  BOOST_CHECK_EQUAL(boost::get<std::string>(seen.at("/data/datatype").m_value), "float32");
  BOOST_CHECK_EQUAL(boost::get<int64_t>(seen.at("/data/shape/0").m_value), 3);
  BOOST_CHECK(seen.find("/data") == seen.end());

  // Nothing is filtered out, including what the writer added
  BOOST_CHECK(seen.count("/asdf_library/name") == 1);

  // Containers are descended into, never emitted as values
  BOOST_CHECK(seen.find("/meta") == seen.end());
  BOOST_CHECK(seen.find("/meta/instrument") == seen.end());
}


BOOST_FIXTURE_TEST_CASE( metadata_for_each_breaks_early, AsdfFileFixture ) {
  auto metadata = openMetadata(metadata_path);

  size_t visited = 0;
  metadata.forEach([&visited](const std::string&, const MetadataEntry&) {
    visited++;
    return false;
  });

  BOOST_CHECK_EQUAL(visited, 1);

  // ... and the walk is repeatable afterwards, i.e. stopping early left nothing broken
  BOOST_CHECK_GT(metadata.toMap().size(), 1);
}


//-----------------------------------------------------------------------------


BOOST_AUTO_TEST_SUITE_END ()
