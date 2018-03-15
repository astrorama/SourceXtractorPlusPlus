#include "SEImplementation/CheckImages/SourceIdCheckImage.h"

#include "SEImplementation/Property/PixelCoordinateList.h"

#include "SEImplementation/CheckImages/CheckImages.h"


namespace SExtractor {

void SourceIdCheckImage::handleMessage(const std::shared_ptr<SourceInterface>& source) {
  if (m_check_image) {
    auto coordinates = source->getProperty<PixelCoordinateList>();

    for (auto& coord : coordinates.getCoordinateList()) {
      m_check_image->setValue(coord.m_x, coord.m_y, m_object_id);
    }
    m_object_id++;
  }
}

}
