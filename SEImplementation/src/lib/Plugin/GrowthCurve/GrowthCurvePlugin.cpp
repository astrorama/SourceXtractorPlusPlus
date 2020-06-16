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

#include <SEFramework/Image/Image.h>
#include "SEFramework/Plugin/StaticPlugin.h"
#include "SEImplementation/Plugin/GrowthCurve/GrowthCurvePlugin.h"
#include "SEImplementation/Plugin/GrowthCurve/GrowthCurveTaskFactory.h"
#include "SEImplementation/Plugin/GrowthCurve/GrowthCurve.h"
#include "SEImplementation/Plugin/GrowthCurve/GrowthCurveResampled.h"

template <typename T>
using NdArray = Euclid::NdArray::NdArray<T>;

namespace SourceXtractor {

static StaticPlugin<GrowthCurvePlugin> growthcurve_plugin;


std::string SourceXtractor::GrowthCurvePlugin::getIdString() const {
  return "GrowthCurvePlugin";
}

void SourceXtractor::GrowthCurvePlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<GrowthCurveTaskFactory, GrowthCurve, GrowthCurveResampled>();

  plugin_api.getOutputRegistry().registerColumnConverter<GrowthCurveResampled, std::vector<double>>(
    "flux_growth_step",
    [](const GrowthCurveResampled& prop){
      return prop.getStepSize();
    },
    "[pixel]",
    "Growth curve step size"
  );
  plugin_api.getOutputRegistry().registerColumnConverter<GrowthCurveResampled, NdArray<DetectionImage::PixelType>>(
    "flux_growth",
    [](const GrowthCurveResampled& prop) {
      return prop.getSamples();
    },
    "[count]",
    "Growth curve samples"
  );

  plugin_api.getOutputRegistry().enableOutput<GrowthCurveResampled>("GrowthCurve");
}

} // end of namespace SourceXtractor