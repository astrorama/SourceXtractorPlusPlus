/*
 * PsfPlugin.h
 *
 *  Created on: Jun 25, 2018
 *      Author: Alejandro Álvarez Ayllón
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_PSF_PSFPLUGIN_H_
#define _SEIMPLEMENTATION_PLUGIN_PSF_PSFPLUGIN_H_

#include <SEFramework/Plugin/Plugin.h>
#include "PsfTaskFactory.h"
#include "PsfProperty.h"

namespace SExtractor {

class PsfPlugin: public Plugin {
public:
  virtual ~PsfPlugin() = default;

  virtual void registerPlugin(PluginAPI& plugin_api) override;

  virtual std::string getIdString() const override;
};

} // end SExtractor

#endif //_SEIMPLEMENTATION_PLUGIN_PSF_PSFPLUGIN_H_
