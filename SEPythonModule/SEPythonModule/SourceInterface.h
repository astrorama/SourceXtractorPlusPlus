/**
 * Copyright © 2022 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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

#ifndef SOURCEXTRACTORPLUSPLUS_SOURCEINTERFACE_H
#define SOURCEXTRACTORPLUSPLUS_SOURCEINTERFACE_H

#include "SEFramework/Source/SourceGroupInterface.h"
#include "SEFramework/Source/SourceInterface.h"
#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEPythonModule/Context.h"
#include <boost/python/dict.hpp>
#include <boost/python/object.hpp>
#include <memory>
#include <string>

namespace SourceXPy {

struct DetachedSource {
  boost::python::dict m_attributes;

  std::string           repr() const;
  boost::python::list   attributes() const;
  boost::python::object attribute(const std::string& key) const;
};

struct AttachedSource {
  ContextPtr                       m_context;
  SourceXtractor::SourceInterface* m_source_ptr = nullptr;

  AttachedSource(ContextPtr context, SourceXtractor::SourceInterface* source_ptr)
      : m_context(std::move(context)), m_source_ptr(source_ptr) {}

  boost::python::object attribute(const std::string& key) const;
  DetachedSource        detach() const;
};

struct OwnedSource : public AttachedSource {
  OwnedSource(ContextPtr context, std::unique_ptr<SourceXtractor::SourceInterface> source)
      : AttachedSource(std::move(context), source.get()), m_owned_source(std::move(source)) {}

  std::string repr() const;

  std::unique_ptr<SourceXtractor::SourceInterface> m_owned_source;

  static std::shared_ptr<OwnedSource> create(const std::shared_ptr<Context>& context, int detection_frame_idx,
                                             int detection_id, const boost::python::tuple& pixels);
};

struct EntangledSource : public AttachedSource {
  explicit EntangledSource(ContextPtr context) : AttachedSource(std::move(context), nullptr) {}

  std::string repr() const;
};

struct SourceGroup {
  struct Iterator {
    SourceXtractor::SourceGroupInterface::const_iterator m_i;
    SourceXtractor::SourceGroupInterface::const_iterator m_end;
    std::shared_ptr<EntangledSource>                     m_holder;

    // It always returns the same!
    std::shared_ptr<EntangledSource> next();
  };

  std::unique_ptr<SourceXtractor::SourceGroupInterface> m_group;
  ContextPtr                                            m_context;

  std::string           repr() const;
  size_t                size() const;
  boost::python::object attribute(const std::string& key) const;
  Iterator              iter() const;
};

}  // namespace SourceXPy

#endif  // SOURCEXTRACTORPLUSPLUS_SOURCEINTERFACE_H
