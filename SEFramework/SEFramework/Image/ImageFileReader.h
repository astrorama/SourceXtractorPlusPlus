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
 * @file ImageFileReader.h
 * @date Aug 14, 2025
 * @author embray
 */

#ifndef _SEFRAMEWORK_IMAGE_IMAGEFILEREADER_H_
#define _SEFRAMEWORK_IMAGE_IMAGEFILEREADER_H_

#include <istream>

#include "SEFramework/Image/ImageSource.h"

namespace SourceXtractor {

/**
 * @class ImageFileReader
 *
 * @brief
 * Registry of supported file types from which ImageSources can be loaded
 *
 * Subclasses should implement the methods for returning / iterating individual ImageSources
 * from a file.
 *
 * @details
 * File type-specific ImageFileReaders (e.g. FitsImageFileReader) can register themselves at
 * compilation time using StaticFileType: This accepts a name for the file format, a vector
 * of supported filename extensions (this is used only as a hint for prioritizing which file
 * type to assume), and a test function that should be able to rapidly check the file type based on
 * its contents, and finally a factory function for constructing the ImageFileReader itself.
 *
 * The test method simply receives an open read-only file stream and should return a boolean
 * if the test passes.
 *
 * The factory method receives the filename which may include an optional [...] suffix containing
 * either an integer or a string.  For example, image.fits[1] receives an image_index 1, which
 * in the case of a FITS file indicates loading the image from the first HDU.
 *
 * Currently only supports opening files for reading, though writing will be a future extension.
 */
class ImageFileReader {
public:
  ImageFileReader(const std::string& filename);
  ImageFileReader(const std::string& filename, const std::string& image_path);
  ImageFileReader(const std::string& filename, int image_index);

  virtual ~ImageFileReader() = default;

  virtual std::shared_ptr<ImageSource> get();
  virtual std::shared_ptr<ImageSource> get(int image_index) = 0;
  virtual std::shared_ptr<ImageSource> get(const std::string& image_path) = 0;

  /* ImageFileReader iterator interface */
  class Iterator {
  public:
    using iterator_category = std::input_iterator_tag;
    using value_type = std::shared_ptr<ImageSource>;
    using difference_type = std::ptrdiff_t;
    using pointer = std::shared_ptr<ImageSource>*;
    using reference = std::shared_ptr<ImageSource>&;

    Iterator(ImageFileReader* reader, int index)
        : m_reader(reader), m_index(index) {
        advance();
    }

    std::shared_ptr<ImageSource> operator*() const { return m_current; }

    Iterator& operator++() {
      ++m_index;
      advance();
      return *this;
    }

    bool operator==(const Iterator& other) const {
      return m_current == other.m_current;
    }

    bool operator!=(const Iterator& other) const {
      return !(*this == other);
    }

  private:
    void advance() {
      try {
        m_current = m_reader->get(m_index);
      } catch (...) {
        m_current = nullptr;
      }
    }

    ImageFileReader* m_reader;
    int m_index;
    std::shared_ptr<ImageSource> m_current;
  };

  Iterator begin() {
    return Iterator(this, 0);
  }

  Iterator end() {
    return Iterator(this, -1);
  }

  /**
   * Test methods receive an open handle to the file (if the file existed) and should read
   * the file contents (e.g. for magic bytes) to check if the file type matches expecations.
   */
  using TestMethod = std::function<bool(std::istream&)>;

  /**
   * Factory methods must return a shared pointer to an ImageFileReader, and receive a string
   * containing the filename
   */
  using FactoryMethod = std::function<std::unique_ptr<ImageFileReader>(const std::string&)>;

  /**
   * @return A list of known file types.
   */
  static std::vector<std::string> getFileTypes();

  /**
   * @return The default file type to use (currently "fits", when in doubt)
   */
  static std::string getDefault();

  /**
   * Create an instance of an `ImageFileReader` from the filename (which may contain an optional
   * extension suffix)
   * @param filename
   *    The filename of the file
   * @return
   *    A new instance of the ImageSource. An Elements::Exception is thrown if the file type
   *    cannot be determined.
   */
  static std::unique_ptr<ImageFileReader> create(const std::string &filename);

  static bool test(std::istream& stream);

  struct ImageFileType {
    std::string name;
    std::vector<std::string> extensions;
    ImageFileReader::TestMethod test;
    ImageFileReader::FactoryMethod factory;
  };

  /**
   * Register a new file type reader
   * @param name
   *    The name of the file type (e.g. "fits"). Case insensitive.
   * @param extensions
   *    Vector of filename extensions (without the .), e.g. "fits", "fit".  If the filename
   *    matches one of these extensions, then this file type will be checked first (this is
   *    only a hint though).
   */
  template <typename Reader>
  struct StaticFileType : ImageFileType {
    StaticFileType(const std::string& name,
                   const std::vector<std::string>& extensions)
      : ImageFileType{
        name, extensions, &Reader::test,
        [](const std::string& filename) { return std::make_unique<Reader>(filename); }
      } {
      registerFileType(*this);
    }
  };

  static void registerFileType(ImageFileType file_type);

protected:
  std::string m_filename;
  std::optional<std::string> m_image_path;
  int m_image_index;
};

}  // end of namespace SourceXtractor

#endif /* _SEFRAMEWORK_IMAGE_IMAGEFILEREADER_H_ */
