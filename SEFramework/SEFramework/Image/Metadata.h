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

/*
 * Metadata.h
 *
 * Keyed metadata read from an image file, independently of its format.
 *
 *  Created on: Oct 28, 2026
 *      Author: embray
 */

#ifndef _SEFRAMEWORK_IMAGE_METADATA_H_
#define _SEFRAMEWORK_IMAGE_METADATA_H_

#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>

#include <boost/variant.hpp>

#include "SEUtils/VariantCast.h"

namespace SourceXtractor {

/**
 * One metadata value, plus whatever the format says about it
 *
 * Accepted values are bool, char, int64_t, double and string; other types must
 * be promoted (i.e. int32_t => int64_t, float => double).
 */
struct MetadataEntry {
  typedef boost::variant<bool, char, int64_t, double, std::string> value_t;

  value_t m_value;

  /// Additional metadata about the entry: i.e. comments
  std::map<std::string, std::string> m_extra;
};

/**
 * Keyed metadata, with each operation left to the implementation
 *
 * Keys are unique, and their shape is the format's own. For example:
 * FITS gives 8-character upper-case keywords, ASDF gives slash-separated tree
 * paths such as "/roman/meta/instrument/optical_element".
 *
 * get() resolves a single key; forEach() enumerates every one.
 */
class Metadata {
public:
  /// Called once per entry by forEach(); returns false to stop the iteration
  using Visitor = std::function<bool(const std::string&, const MetadataEntry&)>;

  virtual ~Metadata() = default;

  /// The value at `key`, or an empty optional if there is none
  virtual std::optional<MetadataEntry> get(const std::string& key) const = 0;

  /// Call `visitor` once per entry, until it returns false
  virtual void forEach(const Visitor& visitor) const = 0;

  /// Whether `key` has a value
  bool contains(const std::string& key) const {
    return get(key).has_value();
  }

  /**
   * The value at `key` converted to T, or an empty optional if there is none
   *
   * Conversion goes through VariantCast, so it is lexical: a string "42" reads
   * as 42 for T = int, and a value that cannot be converted throws.
   */
  template <typename T>
  std::optional<T> getAs(const std::string& key) const {
    auto entry = get(key);

    if (!entry) {
      return std::nullopt;
    }

    return VariantCast<T>(entry->m_value);
  }

  /// Shared instance holding nothing, so callers need not handle a null pointer
  static std::shared_ptr<const Metadata> empty();

  /// Every entry, collected into a map
  std::map<std::string, MetadataEntry> toMap() const {
    std::map<std::string, MetadataEntry> out;
    forEach([&out](const std::string& key, const MetadataEntry& entry) {
      out.emplace(key, entry);
      return true;
    });
    return out;
  }
};

/**
 * Metadata already held in a std::map
 *
 * The backing for FITS headers, and for anything that carries metadata around
 * beyond the lifetime of the image from which it came.
 */
class MapMetadata : public Metadata {
public:
  MapMetadata() = default;

  explicit MapMetadata(std::map<std::string, MetadataEntry> entries)
    : m_entries(std::move(entries)) {}

  std::optional<MetadataEntry> get(const std::string& key) const override {
    auto found = m_entries.find(key);

    if (found == m_entries.end()) {
      return std::nullopt;
    }

    return found->second;
  }

  void forEach(const Visitor& visitor) const override {
    for (const auto& entry : m_entries) {
      if (!visitor(entry.first, entry.second)) {
        return;
      }
    }
  }

  /// Add or replace one entry
  void set(const std::string& key, const MetadataEntry& value) {
    m_entries[key] = value;
  }

  size_t size() const {
    return m_entries.size();
  }

private:
  std::map<std::string, MetadataEntry> m_entries;
};

}  // namespace SourceXtractor

#endif /* _SEFRAMEWORK_IMAGE_METADATA_H_ */
