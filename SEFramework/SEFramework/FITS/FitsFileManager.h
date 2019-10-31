/*
 * FitsFileManager.h
 *
 *  Created on: Sep 19, 2019
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_FITS_FITSFILEMANAGER_H_
#define _SEFRAMEWORK_FITS_FITSFILEMANAGER_H_

#include <memory>
#include <string>
#include <list>
#include <unordered_map>

#include <fitsio.h>


namespace SourceXtractor {

class FitsFileManager {
public:

  FitsFileManager();
  virtual ~FitsFileManager();

  fitsfile* getFitsFile(const std::string& filename, bool writeable = false);

  void closeAllFiles();

  static std::shared_ptr<FitsFileManager> getInstance() {
    if (s_instance == nullptr) {
      s_instance = std::make_shared<FitsFileManager>();
    }
    return s_instance;
  }

private:
  fitsfile* openFitsFile(const std::string& filename, bool writeable) const;
  void closeFitsFile(fitsfile* fptr) const;
  void closeExtraFiles();

  struct FitsInfo {
    fitsfile* m_file_pointer;
    bool m_is_file_opened;
    bool m_is_writeable;
  };

  std::unordered_map<std::string, FitsInfo> m_fits_files;

  unsigned int m_max_open_files;
  std::list<std::string> m_open_files;

  static std::shared_ptr<FitsFileManager> s_instance;
};

}

#endif /* _SEFRAMEWORK_FITS_FITSFILEMANAGER_H_ */
