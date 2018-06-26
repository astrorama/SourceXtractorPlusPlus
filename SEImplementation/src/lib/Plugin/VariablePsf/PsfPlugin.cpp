/*
 * PsfPlugin.cpp
 *
 *  Created on: Jun 25, 2018
 *      Author: Alejandro Álvarez Ayllón
 */

#include <SEFramework/Plugin/StaticPlugin.h>
#include "SEImplementation/Plugin/Psf/PsfPlugin.h"

namespace SExtractor {

void PsfPlugin::registerPlugin(SExtractor::PluginAPI &plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<PsfTaskFactory, PsfProperty>();

  plugin_api.getOutputRegistry().registerColumnConverter<PsfProperty, std::vector<float>>(
      "psf",
      [](const PsfProperty &prop) -> std::vector<float> {
        return prop.getPsf().getScaledKernel(1)->getData();
      }
  );

  plugin_api.getOutputRegistry().optionalOutput<PsfProperty>("Psf");
}

std::string PsfPlugin::getIdString() const  {
  return "psf";
}

static StaticPlugin<PsfPlugin> psf_plugin;
}
