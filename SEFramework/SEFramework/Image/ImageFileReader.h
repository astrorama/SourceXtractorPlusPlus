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

#include "SEFramework/Image/BufferedImage.h"
#include "SEFramework/Image/ImageSelector.h"
#include "SEFramework/Image/ImageSource.h"
#include "SEFramework/Image/ImageTile.h"

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

  /**
   * Equivalent to calling get() with whatever m_selector names
   */
  std::shared_ptr<ImageSource> get(ImageTile::ImageType image_type = ImageTile::AutoType);

  /**
   * Get one image by the format's own numeric address
   *
   * The meaning is the format's, as it is for get(const std::string&): a FITS
   * index is the HDU number counting the primary HDU as 0, and raises for an
   * HDU holding no image; an ASDF index is the array's position among the
   * images.  at() is the one that indexes the images themselves.
   */
  virtual std::shared_ptr<ImageSource> get(
    int image_index, ImageTile::ImageType image_type = ImageTile::AutoType) = 0;
  virtual std::shared_ptr<ImageSource> get(
    const std::string& image_path, ImageTile::ImageType image_type = ImageTile::AutoType) = 0;

  /**
   * Get the image at a position in iteration order, as vector::at does
   *
   * Used by iter(), and to pick a file's first image when nothing named one.
   * Throws when the file offers no such image.  The default suits a format
   * whose numeric address already counts images; FitsReader overrides it.
   */
  virtual std::shared_ptr<ImageSource> at(
    int n, ImageTile::ImageType image_type = ImageTile::AutoType) {
    return get(n, image_type);
  }

  /**
   * Select which image within the file to use, unless the filename already said
   *
   * Takes the same text as a file[image] suffix: an integer index, or a path whose
   * meaning is the format's own (an ASDF tree path, a FITS EXTNAME).  Intended for a
   * configured default covering many files, so an explicit suffix on one filename
   * takes precedence and this is then ignored.
   */
  void setImageName(const std::string& image_name);

  /**
   * Whether this file format can select a coordinate system by name
   *
   * True for formats that may hold several coordinate systems with no fixed
   * association to the image arrays, so that one has to be named explicitly.
   * That is the case for ASDF; it would also be the case for FITS carrying an
   * embedded GWCS, which is not supported yet.
   */
  virtual bool supportsWcsName() const { return false; }

  /**
   * Select which coordinate system the images from this reader should use
   *
   * Applies to every ImageSource subsequently returned by get() or iter().
   * Ignored, with a warning, by formats that do not support it.
   */
  void setWcsName(std::optional<std::string> wcs_name);

  const std::optional<std::string>& getWcsName() const {
    return m_wcs_name;
  }

  /**
   * The name of the file format this reader was created for, e.g. "fits"
   *
   * Set by create(), which may pick the format by content rather than by
   * filename extension, so this is the authority on what the file actually is.
   */
  const std::string& getFormat() const {
    return m_format;
  }

  /**
   * The names by which the images in this file can be addressed, in file order
   *
   * Each name is accepted by get(const std::string&).  An image that has no
   * name gets an empty entry, which can only be reached by index.  Names are
   * the format's own: a FITS EXTNAME, or an ASDF tree path, etc.
   */
  virtual std::vector<std::string> getImageNames() {
    return {};
  }

  /* ImageFileReader iterator interface */
  class Iterator {
  public:
    using iterator_category = std::input_iterator_tag;
    using value_type = std::shared_ptr<ImageSource>;
    using difference_type = std::ptrdiff_t;
    using pointer = std::shared_ptr<ImageSource>*;
    using reference = std::shared_ptr<ImageSource>&;

    Iterator() noexcept : m_reader(nullptr), m_index(0), m_image_type(ImageTile::AutoType) {}

    Iterator(ImageFileReader* reader, int index,
             ImageTile::ImageType image_type = ImageTile::AutoType)
        : m_reader(reader), m_index(index), m_image_type(image_type) {
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

  /* To support ImageFileReader->iter(ImageTile::ImageType) */
  Iterator begin() {
    return *this;
  }

  Iterator end() {
    return Iterator();
  }

  private:
    /**
     * Fetch the image at the current position, leaving it null once done
     */
    void advance();

    ImageFileReader* m_reader;
    int m_index;
    std::shared_ptr<ImageSource> m_current;
    ImageTile::ImageType m_image_type;
  };

  Iterator iter(ImageTile::ImageType image_type = ImageTile::AutoType) {
    return Iterator(this, 0, image_type);
  }

  Iterator begin() {
    return Iterator(this, 0, ImageTile::AutoType);
  }

  Iterator end() {
    return Iterator();
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
   * Convenience shortcut for returning a buffered image of a given datatype
   * the file
   *
   * @param filename
   *    The filename of the file
   * @param image_index (optional)
   *    The index of the image to return from the file if it contains multiple
   *    images (such as a multi-extension FITS file)
   * @return
   *    A new instance of an Image<T>. An Elements::Exception is thrown if the file type
   *    cannot be determined or does not contain a valid image.
   */
  template <typename T>
  static std::shared_ptr<Image<T>> readImage(const std::string& filename) {
    auto reader = create(filename);
    auto image_source = reader->get(ImageTile::getTypeValue(T()));
    return BufferedImage<T>::create(image_source);
  }

  template <typename T>
  static std::shared_ptr<Image<T>> readImage(const std::string& filename, int image_index) {
    auto reader = create(filename);
    auto image_source = reader->get(image_index, ImageTile::getTypeValue(T()));
    return BufferedImage<T>::create(image_source);
  }

  /**
   * Convenience shortcut for returning a buffered image of a given datatype
   * the file
   *
   * @param filename
   *    The filename of the file
   * @param image_path
   *    The name or path within the file for the image if there are multiple
   *    images in the file (e.g. the EXTNAME of a FITS HDU or a path within
   *    an ASDF file)
   * @return
   *    A new instance of an Image<T>. An Elements::Exception is thrown if the file type
   *    cannot be determined or does not contain a valid image.
   */
  template <typename T>
  static std::shared_ptr<Image<T>> readImage(const std::string& filename,
                                             const std::string& image_path) {
    auto reader = create(filename);
    auto image_source = reader->get(image_path, ImageTile::getTypeValue(T()));
    return BufferedImage<T>::create(image_source);
  }

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

private:
  /// Creates a reader if file_type recognizes the file, stamping it with the format name
  static std::unique_ptr<ImageFileReader> tryCreateFromFile(
    const std::string& base_filename, const std::string& filename,
    ImageFileType& file_type);

protected:
  /**
   * Whether the file[image] suffix selects which image(s) in the file are used
   *
   * get() and iter() then cover only the selection, rather than defaulting to
   * the first image or walking the whole file.  A selection names a single
   * image today, by index or by name, so iterating one yields exactly one
   * image; it need not stay that narrow (e.g. one could imagine a wildcard
   * selection).
   */
  bool hasImageSelection() const {
    return m_selector.isSpecified();
  }

  std::string m_filename;
  ImageSelector m_selector;
  std::optional<std::string> m_wcs_name;
  std::string m_format;
};

}  // end of namespace SourceXtractor

#endif /* _SEFRAMEWORK_IMAGE_IMAGEFILEREADER_H_ */
