/*
 * JacobianTask.cpp
 *
 *  Created on: Oct 08, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#include "SEFramework/Property/DetectionFrame.h"
#include "SEImplementation/Plugin/DetectionFrameGroupStamp/DetectionFrameGroupStamp.h"
#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h"
#include "SEImplementation/Plugin/DetectionFrameSourceStamp/DetectionFrameSourceStamp.h"
#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFrame.h"
#include "SEImplementation/Plugin/Jacobian/JacobianTask.h"
#include "SEImplementation/Plugin/Jacobian/Jacobian.h"

namespace SExtractor {

void JacobianGroupTask::computeProperties(SourceGroupInterface &group) const {
  auto frame = group.begin()->getProperty<MeasurementFrame>(m_instance).getFrame();
  auto frame_coordinates = frame->getCoordinateSystem();
  auto& detection_group_stamp = group.getProperty<DetectionFrameGroupStamp>();
  auto detection_frame_coordinates = group.begin()->getProperty<DetectionFrame>().getFrame()->getCoordinateSystem();

  double x = detection_group_stamp.getTopLeft().m_x + detection_group_stamp.getStamp().getWidth() / 2.0;
  double y = detection_group_stamp.getTopLeft().m_y + detection_group_stamp.getStamp().getHeight() / 2.0;

  auto frame_origin = frame_coordinates->worldToImage(detection_frame_coordinates->imageToWorld(ImageCoordinate(x, y)));
  auto frame_dx = frame_coordinates->worldToImage(
    detection_frame_coordinates->imageToWorld(ImageCoordinate(x + 1.0, y)));
  auto frame_dy = frame_coordinates->worldToImage(
    detection_frame_coordinates->imageToWorld(ImageCoordinate(x, y + 1.0)));

  group.setIndexedProperty<JacobianGroup>(m_instance,
                                          frame_dx.m_x - frame_origin.m_x, frame_dx.m_y - frame_origin.m_y,
                                          frame_dy.m_x - frame_origin.m_x, frame_dy.m_y - frame_origin.m_y);
}

void JacobianSourceTask::computeProperties(SourceInterface &source) const {
  auto frame = source.getProperty<MeasurementFrame>(m_instance).getFrame();
  auto frame_coordinates = frame->getCoordinateSystem();
  auto& detection_boundaries = source.getProperty<PixelBoundaries>();
  auto detection_frame_coordinates = source.getProperty<DetectionFrame>().getFrame()->getCoordinateSystem();

  double x = detection_boundaries.getMin().m_x + detection_boundaries.getWidth() / 2.0;
  double y = detection_boundaries.getMin().m_y + detection_boundaries.getHeight() / 2.0;

  auto frame_origin = frame_coordinates->worldToImage(detection_frame_coordinates->imageToWorld(ImageCoordinate(x, y)));
  auto frame_dx = frame_coordinates->worldToImage(
    detection_frame_coordinates->imageToWorld(ImageCoordinate(x + 1.0, y)));
  auto frame_dy = frame_coordinates->worldToImage(
    detection_frame_coordinates->imageToWorld(ImageCoordinate(x, y + 1.0)));

  source.setIndexedProperty<JacobianSource>(m_instance,
                                            frame_dx.m_x - frame_origin.m_x, frame_dx.m_y - frame_origin.m_y,
                                            frame_dy.m_x - frame_origin.m_x, frame_dy.m_y - frame_origin.m_y);
}

} // end SExtractor
