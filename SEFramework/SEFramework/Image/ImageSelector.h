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
 * @file ImageSelector.h
 * @author embray
 *
 * Which image within a file to use.
 */

#ifndef _SEFRAMEWORK_IMAGE_IMAGESELECTOR_H
#define _SEFRAMEWORK_IMAGE_IMAGESELECTOR_H

#include <stdexcept>
#include <string>
#include <variant>

namespace SourceXtractor {

/**
 * Which image within a possibly multi-image file to use: an index, a name, or
 * neither
 *
 * One concept with two spellings, which is how a file[selector] suffix and the
 * --*-image-name options both come to accept either. What an index means
 * depends entirely on the underlying file format.  So for FITS this keeps the
 * existing behavior of selecting an HDU by sequential index.
 *
 * Selecting nothing means the file's only image, or the first of several, if
 * it's unambiguous for that file (also a question dependent on file format).
 */
class ImageSelector {

public:

  /// Selects nothing in particular
  ImageSelector() = default;

  explicit ImageSelector(int index) : m_selector(index) {}

  explicit ImageSelector(std::string name) : m_selector(std::move(name)) {}

  /// An index if the whole text parses as one, a name otherwise
  static ImageSelector parse(const std::string& text) {
    try {
      size_t end;
      int index = std::stoi(text, &end);

      if (end == text.size()) {
        return ImageSelector{index};
      }
    } catch (const std::logic_error&) {
      // Not an integer, so a name
    }

    return ImageSelector{text};
  }

  bool isSpecified() const {
    return !std::holds_alternative<std::monostate>(m_selector);
  }

  bool isIndex() const {
    return std::holds_alternative<int>(m_selector);
  }

  bool isName() const {
    return std::holds_alternative<std::string>(m_selector);
  }

  /// Only meaningful when isIndex()
  int getIndex() const {
    return std::get<int>(m_selector);
  }

  /// Only meaningful when isName()
  const std::string& getName() const {
    return std::get<std::string>(m_selector);
  }

  /// As it would be written in a file[selector] suffix, empty if unspecified
  std::string toString() const {
    if (isIndex()) {
      return std::to_string(getIndex());
    }

    if (isName()) {
      return getName();
    }

    return "";
  }

  bool operator==(const ImageSelector& other) const {
    return m_selector == other.m_selector;
  }

  bool operator!=(const ImageSelector& other) const {
    return !(*this == other);
  }

private:
  std::variant<std::monostate, int, std::string> m_selector;
};

}  // namespace SourceXtractor

#endif /* _SEFRAMEWORK_IMAGE_IMAGESELECTOR_H */
