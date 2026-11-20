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
#include <future>

#include <boost/test/unit_test.hpp>

#include "ElementsKernel/Exception.h"
#include "SEFramework/FITS/TemporaryFitsImageSource.h"
#include "SEFramework/Image/WriteableBufferedImage.h"
#include "SEImplementation/Image/LockedWriteableImage.h"

using namespace SourceXtractor;

BOOST_AUTO_TEST_SUITE(LockedWriteableImage_test)

BOOST_AUTO_TEST_CASE(WritePinFollowsWrapperLifetime) {
  auto manager = std::make_shared<TileManager>();
  manager->setOptions(512, 512, 1);
  auto source = std::make_shared<TemporaryFitsImageSource>(512, 512, ImageTile::FloatImage);
  auto other_source = std::make_shared<TemporaryFitsImageSource>(512, 512, ImageTile::FloatImage);
  auto image = WriteableBufferedImage<float>::create(source, manager);
  auto other_image = BufferedImage<float>::create(other_source, manager);

  {
    auto locked_image = LockedWriteableImage<float>::create(image);
    locked_image->setValue(0, 0, 1.f);

    // The wrapper must keep its tile pinned between writes as other threads load tiles.
    auto cache_pressure = std::async(std::launch::async, [&] {
      other_image->getChunk(0, 0, 1, 1);
    });
    cache_pressure.get();

    BOOST_CHECK_THROW(manager->flush(), Elements::Exception);
    BOOST_CHECK_EQUAL(source->getImageTile(0, 0, 1, 1)->getValue<float>(0, 0), 0.f);
    locked_image->setValue(0, 0, 2.f);
  }

  // Loading another tile can now evict and save the tile that the wrapper pinned.
  other_image->getChunk(0, 0, 1, 1);
  BOOST_CHECK_EQUAL(source->getImageTile(0, 0, 1, 1)->getValue<float>(0, 0), 2.f);
  BOOST_CHECK_NO_THROW(manager->flush());
}

BOOST_AUTO_TEST_SUITE_END()
