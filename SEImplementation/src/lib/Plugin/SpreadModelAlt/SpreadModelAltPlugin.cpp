/** Copyright © 2019-2026 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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
#include "SEImplementation/Plugin/SpreadModelAlt/SpreadModelAlt.h"
#include "SEImplementation/Plugin/SpreadModelAlt/CombinedSpreadModelAlt.h"
#include "SEImplementation/Plugin/SpreadModelAlt/SpreadModelAltPlugin.h"
#include "SEImplementation/Plugin/SpreadModelAlt/SpreadModelAltTaskFactory.h"
#include "SEFramework/Plugin/StaticPlugin.h"
#include "SEImplementation/Plugin/SpreadModelAlt/JointSpreadModelAlt.h"

namespace SourceXtractor {

static StaticPlugin<SpreadModelAltPlugin> spread_model_alt_plugin;

void SpreadModelAltPlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry()
      .registerTaskFactory<SpreadModelAltTaskFactory, SpreadModelAlt, CombinedSpreadModelAlt, JointSpreadModelAlt>();

  plugin_api.getOutputRegistry().registerColumnConverter<SpreadModelAlt, double>(
          "spread_model_alt",
          [](const SpreadModelAlt& prop) {
            return prop.getSpreadModelAlt();
          },
          "",
          "Spread model"
  );

  plugin_api.getOutputRegistry().registerColumnConverter<SpreadModelAlt, double>(
          "spread_model_alt_error",
          [](const SpreadModelAlt& prop) {
            return prop.getSpreadModelAltError();
          },
          "",
          "Spread model error"
  );

  plugin_api.getOutputRegistry().enableOutput<SpreadModelAlt>("SpreadModelAlt");

  auto& registry = plugin_api.getOutputRegistry();
  registry.registerColumnConverter<CombinedSpreadModelAlt, double>("combined_spread_model_alt",
      [](const CombinedSpreadModelAlt& prop) { return prop.getSpreadModelAlt(); }, "",
      "Inverse-variance weighted spread model across measurement frames");
  registry.registerColumnConverter<CombinedSpreadModelAlt, double>("combined_spread_model_alt_error",
      [](const CombinedSpreadModelAlt& prop) { return prop.getSpreadModelAltError(); }, "",
      "Formal combined spread uncertainty assuming independent frames");
  registry.registerColumnConverter<CombinedSpreadModelAlt, int>("combined_spread_model_alt_nframes",
      [](const CombinedSpreadModelAlt& prop) { return prop.getNFrames(); }, "",
      "Number of valid frames in the combined spread model");
  registry.registerColumnConverter<CombinedSpreadModelAlt, double>("combined_spread_model_alt_reduced_chi_squared",
      [](const CombinedSpreadModelAlt& prop) { return prop.getReducedChiSquared(); }, "",
      "Reduced chi-square of the per-frame spread measurements (NaN for fewer than two frames)");
  registry.enableOutput<CombinedSpreadModelAlt>("CombinedSpreadModelAlt");
  registry.registerColumnConverter<JointSpreadModelAlt, double>("joint_spread_model_alt_radius",
      [](const JointSpreadModelAlt& prop) { return prop.getResult().radius; }, "pixel",
      "Joint intrinsic exponential half-light radius in reference-image pixels");
  registry.registerColumnConverter<JointSpreadModelAlt, double>("joint_spread_model_alt_radius_error",
      [](const JointSpreadModelAlt& prop) { return prop.getResult().radius_error; }, "pixel",
      "Formal radius uncertainty; NaN at a radius boundary");
  registry.registerColumnConverter<JointSpreadModelAlt, double>("joint_spread_model_alt_delta_chi_squared",
      [](const JointSpreadModelAlt& prop) { return prop.getResult().delta_chi_squared; }, "",
      "Point minus extended chi-square; not a calibrated significance");
  registry.registerColumnConverter<JointSpreadModelAlt, double>("joint_spread_model_alt_point_chi_squared",
      [](const JointSpreadModelAlt& prop) { return prop.getResult().point_chi_squared; }, "",
      "Joint point-source pixel chi-square");
  registry.registerColumnConverter<JointSpreadModelAlt, double>("joint_spread_model_alt_extended_chi_squared",
      [](const JointSpreadModelAlt& prop) { return prop.getResult().extended_chi_squared; }, "",
      "Joint extended-source pixel chi-square including point limit");
  registry.registerColumnConverter<JointSpreadModelAlt, int>("joint_spread_model_alt_nframes",
      [](const JointSpreadModelAlt& prop) { return prop.getResult().n_frames; }, "",
      "Number of frames shared by both joint hypotheses");
  registry.registerColumnConverter<JointSpreadModelAlt, int>("joint_spread_model_alt_npixels",
      [](const JointSpreadModelAlt& prop) { return prop.getResult().n_pixels; }, "",
      "Number of unmasked pixels shared by both joint hypotheses");
  registry.registerColumnConverter<JointSpreadModelAlt, int>("joint_spread_model_alt_flags",
      [](const JointSpreadModelAlt& prop) { return prop.getResult().flags; }, "",
      "Joint flags: 1 error, 2 iteration limit, 4 radius boundary, 8 outside, 16 insufficient data");
  registry.registerColumnConverter<JointSpreadModelAlt, double>("joint_spread_model_alt_point_prior_penalty",
      [](const JointSpreadModelAlt& prop) { return prop.getResult().point_prior_penalty; }, "",
      "Point-fit sum of squared normalized position offsets");
  registry.registerColumnConverter<JointSpreadModelAlt, double>("joint_spread_model_alt_extended_prior_penalty",
      [](const JointSpreadModelAlt& prop) { return prop.getResult().extended_prior_penalty; }, "",
      "Extended-fit sum of squared normalized position offsets");
  registry.registerColumnConverter<JointSpreadModelAlt, double>("joint_spread_model_alt_delta_objective",
      [](const JointSpreadModelAlt& prop) { return prop.getResult().delta_objective; }, "",
      "Point minus extended penalized objective, including position priors");
  registry.registerColumnConverter<JointSpreadModelAlt, double>("joint_spread_model_alt_point_offset_rms",
      [](const JointSpreadModelAlt& prop) { return prop.getResult().point_offset_rms; }, "pixel",
      "Point-fit RMS radial frame offset in reference pixels");
  registry.registerColumnConverter<JointSpreadModelAlt, double>("joint_spread_model_alt_extended_offset_rms",
      [](const JointSpreadModelAlt& prop) { return prop.getResult().extended_offset_rms; }, "pixel",
      "Extended-fit RMS radial frame offset in reference pixels");
  registry.registerColumnConverter<JointSpreadModelAlt, std::vector<int>>("joint_spread_model_alt_frame_ids",
      [](const JointSpreadModelAlt& prop) { return prop.getResult().frame_ids; }, "",
      "Configured frame IDs defining the position-offset array order");
  registry.registerColumnConverter<JointSpreadModelAlt, std::vector<double>>("joint_spread_model_alt_point_offset_x",
      [](const JointSpreadModelAlt& prop) { return prop.getResult().point_offset_x; }, "pixel",
      "Point-fit reference x offsets; NaN for excluded frames");
  registry.registerColumnConverter<JointSpreadModelAlt, std::vector<double>>("joint_spread_model_alt_point_offset_y",
      [](const JointSpreadModelAlt& prop) { return prop.getResult().point_offset_y; }, "pixel",
      "Point-fit reference y offsets; NaN for excluded frames");
  registry.registerColumnConverter<JointSpreadModelAlt, std::vector<double>>("joint_spread_model_alt_extended_offset_x",
      [](const JointSpreadModelAlt& prop) { return prop.getResult().extended_offset_x; }, "pixel",
      "Extended-fit reference x offsets; NaN for excluded frames");
  registry.registerColumnConverter<JointSpreadModelAlt, std::vector<double>>("joint_spread_model_alt_extended_offset_y",
      [](const JointSpreadModelAlt& prop) { return prop.getResult().extended_offset_y; }, "pixel",
      "Extended-fit reference y offsets; NaN for excluded frames");
  registry.enableOutput<JointSpreadModelAlt>("CombinedSpreadModelAlt");

}

std::string SpreadModelAltPlugin::getIdString() const {
  return "SpreadModelAlt";
}

}
