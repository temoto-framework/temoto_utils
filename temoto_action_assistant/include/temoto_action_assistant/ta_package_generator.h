/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright 2019 TeMoto Telerobotics
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef TEMOTO_ACTION_ENGINE__TA_PACKAGE_GENERATOR_H
#define TEMOTO_ACTION_ENGINE__TA_PACKAGE_GENERATOR_H

#include "file_template_parser/file_template_parser.h"
#include "temoto_action_engine/umrf_node.h"
#include "temoto_action_engine/umrf_graph.h"
#include "temoto_action_engine/umrf_json_converter.h"

namespace temoto_action_assistant
{
class ActionPackageGenerator
{
public:
  ActionPackageGenerator(const std::string& file_template_path);
  void generatePackage(const UmrfNode& umrf, const std::string& package_path);
  void generateGraph(const UmrfGraph& umrf_graph, const std::string& graphs_path);

private:
  std::string file_template_path_;
  bool file_templates_loaded_;
  /*
   * Templates
   */
  tp::TemplateContainer t_cmakelists;
  tp::TemplateContainer t_packagexml;
  tp::TemplateContainer t_testlaunch_standalone;
  tp::TemplateContainer t_testlaunch_separate;
  tp::TemplateContainer t_umrf_graph;
  tp::TemplateContainer t_macros_header;
  tp::TemplateContainer t_bridge_header;
  tp::TemplateContainer t_bridge_header_elif;

  tp::TemplateContainer t_class_base;
  tp::TemplateContainer t_execute_action;
  tp::TemplateContainer t_get_input_params;
  tp::TemplateContainer t_set_output_params;
  tp::TemplateContainer t_parameter_in;
  tp::TemplateContainer t_parameter_out;
  tp::TemplateContainer t_parameter_decl;
  tp::TemplateContainer t_comment;
  tp::TemplateContainer t_line_comment;
};
} // temoto_action_assistant namespace
#endif