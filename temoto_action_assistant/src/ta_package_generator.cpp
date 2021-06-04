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

#include "temoto_action_assistant/ta_package_generator.h"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <set>

namespace temoto_action_assistant
{
ActionPackageGenerator::ActionPackageGenerator(const std::string& file_template_path)
: file_template_path_(file_template_path + "/")
, file_templates_loaded_(false)
{
  if (file_template_path_.empty())
  {
    return;
  }

  // Import the CMakeLists template
  t_cmakelists = tp::TemplateContainer(file_template_path_ + "file_templates/temoto_ta_cmakelists.xml");

  // Import the package.xml template
  t_packagexml = tp::TemplateContainer(file_template_path_ + "file_templates/temoto_ta_packagexml.xml");

  // Import the action test templates
  //t_testlaunch_standalone = tp::TemplateContainer(file_template_path_ + "file_templates/temoto_ta_action_test_standalone.xml");
  t_testlaunch_separate   = tp::TemplateContainer(file_template_path_ + "file_templates/temoto_ta_action_test_separate.xml");
  t_umrf_graph            = tp::TemplateContainer(file_template_path_ + "file_templates/temoto_ta_umrf_graphtxt.xml");
  
  // Import the macros.h template
  t_macros_header  = tp::TemplateContainer(file_template_path_ + "file_templates/temoto_ta_macros_header.xml");

  // Import the temoto_action.h template
  t_bridge_header  = tp::TemplateContainer(file_template_path_ + "file_templates/temoto_ta_bridge_header.xml");
  t_bridge_header_elif = tp::TemplateContainer(file_template_path_ + "file_templates/temoto_ta_update_params_elif.xml");

  // Import the action implementation c++ code templates
  t_class_base        = tp::TemplateContainer(file_template_path_ + "file_templates/ta_class_base.xml");
  t_execute_action    = tp::TemplateContainer(file_template_path_ + "file_templates/ta_execute_action.xml");
  t_get_input_params  = tp::TemplateContainer(file_template_path_ + "file_templates/ta_get_input_params.xml");
  t_set_output_params = tp::TemplateContainer(file_template_path_ + "file_templates/ta_set_output_params.xml");
  t_parameter_in      = tp::TemplateContainer(file_template_path_ + "file_templates/ta_parameter_in.xml");
  t_parameter_out     = tp::TemplateContainer(file_template_path_ + "file_templates/ta_parameter_out.xml");
  t_parameter_decl    = tp::TemplateContainer(file_template_path_ + "file_templates/ta_parameter_decl.xml");
  t_line_comment      = tp::TemplateContainer(file_template_path_ + "file_templates/ta_line_comment.xml");

  file_templates_loaded_ = true;
}

void ActionPackageGenerator::generatePackage(const UmrfNode& umrf, const std::string& package_path)
{
  if (!file_templates_loaded_)
  {
    std::cout << "Could not generate a TeMoto action package because the file templates are not loaded" << std::endl;
    return;
  }

  /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
   *                    CREATE AI PACKAGE DIRECTORY STRUCTURE
   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

  // Get the name of the package
  const std::string ta_package_name = umrf.getPackageName();
  const std::string ta_dst_path = package_path + "/" + ta_package_name + "/";

  // Create a package directory
  boost::filesystem::create_directories(ta_dst_path + "/src");
  boost::filesystem::create_directories(ta_dst_path + "/launch");
  boost::filesystem::create_directories(ta_dst_path + "/test");
  boost::filesystem::create_directories(ta_dst_path + "/include/" + ta_package_name);

  /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
   *                           GENERATE THE CONTENT
   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

  /*
   * Extract the necessary datastructures from the umrf
   */
  std::string ta_class_name = umrf.getName();

  /*
   * Generate umrf.json
   */
  std::ofstream umrf_json_file;
  umrf_json_file.open (ta_dst_path + "/umrf.json");
  umrf_json_file << umrf_json_converter::toUmrfJsonStr(umrf, true);
  umrf_json_file.close();

  /*
   * Generate invoker umrf graph
   */
  UmrfNode invoker_umrf = umrf;
  invoker_umrf.getInputParametersNc().clear();

  // Give input parameters some dummy values
  for (auto& param_in : umrf.getInputParameters())
  {
    if (param_in.getType() == "string")
    {
      ActionParameters::ParameterContainer pc = param_in;
      pc.setData(boost::any(std::string("MODIFY THIS FIELD")));
      invoker_umrf.getInputParametersNc().setParameter(pc, true);
    }
    else if (param_in.getType() == "number")
    {
      ActionParameters::ParameterContainer pc = param_in;
      pc.setData(boost::any(double(0.0)));
      invoker_umrf.getInputParametersNc().setParameter(pc, true);
    }
  }

  invoker_umrf.setSuffix(0);
  UmrfGraph invoker_umrf_graph(ta_package_name, std::vector<UmrfNode>{invoker_umrf});
  generateGraph(invoker_umrf_graph, ta_dst_path + "/test");

  /*
   * Generate CMakeLists.txt
   */
  t_cmakelists.setArgument("ta_name", ta_package_name);
  t_cmakelists.processAndSaveTemplate(ta_dst_path, "CMakeLists");

  /*
   * Generate package.xml
   */
  t_packagexml.setArgument("ta_name", ta_package_name);
  t_packagexml.processAndSaveTemplate(ta_dst_path, "package");

  /*
   * Generate the temoto_action header
   */
  t_bridge_header.setArgument("ta_package_name", ta_package_name);
  std::ofstream bridge_header_file;
  bridge_header_file.open (ta_dst_path + "/include/" + ta_package_name + "/temoto_action.h");
  bridge_header_file << t_bridge_header.processTemplate();
  bridge_header_file.close();

  /*
   * Generate invoke_action.launch 
   */
  t_testlaunch_separate.setArgument("ta_package_name", ta_package_name);
  t_testlaunch_separate.processAndSaveTemplate(ta_dst_path + "launch/", "invoke_action");

  /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
   * Generate the action implementation c++ source file
   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

  std::string generated_content_cpp;
  std::set<std::string> input_param_update_set;

  /*
   * Generate the "getInputParameters()" function
   */
  std::string gen_content_get_input_params;
  for (const auto& input_param : umrf.getInputParameters())
  {
    std::string parameter_name = "in_param_" + input_param.getName();
    boost::replace_all(parameter_name, "::", "_");
    t_parameter_in.setArgument("param_name", input_param.getName());
    t_parameter_in.setArgument("param_name_us", parameter_name);
    t_parameter_in.setArgument("param_type", input_param.getType());
    t_bridge_header_elif.setArgument("param_type", input_param.getType());

    if (action_parameter::PARAMETER_MAP.find(input_param.getType()) != action_parameter::PARAMETER_MAP.end())
    {
      t_parameter_in.setArgument("param_type_us", action_parameter::PARAMETER_MAP.at(input_param.getType()));
      t_bridge_header_elif.setArgument("param_type_us", action_parameter::PARAMETER_MAP.at(input_param.getType()));
    }
    else
    {
      t_parameter_in.setArgument("param_type_us", input_param.getType());
      t_bridge_header_elif.setArgument("param_type_us", input_param.getType());
    }
    gen_content_get_input_params += "  " + t_parameter_in.processTemplate() + "\n";
    input_param_update_set.insert(t_bridge_header_elif.processTemplate());
  }
  t_get_input_params.setArgument("function_body", gen_content_get_input_params);

  /*
   * Generate the "setOutputParameters()" function
   */
  std::string gen_content_set_output_params;
  for (const auto& output_param : umrf.getOutputParameters())
  {
    std::string parameter_name = "out_param_" + output_param.getName();
    boost::replace_all(parameter_name, "::", "_");
    t_parameter_out.setArgument("param_name_us", parameter_name);
    t_parameter_out.setArgument("param_name", output_param.getName());
    t_parameter_out.setArgument("param_type", output_param.getType());

    // if (action_parameter::PARAMETER_MAP.find(output_param.getType()) != action_parameter::PARAMETER_MAP.end())
    // {
    //   t_parameter_out.setArgument("param_type_us", action_parameter::PARAMETER_MAP.at(output_param.getType()));
    // }
    // else
    // {
    //   t_parameter_out.setArgument("param_type_us", output_param.getType());
    // }

    gen_content_set_output_params += "  " + t_parameter_out.processTemplate() + "\n";
  }
  t_set_output_params.setArgument("function_body", gen_content_set_output_params);

  /*
   * Declare input parameters
   */
  std::string gen_content_input_param_decl;
  if (!umrf.getInputParameters().empty())
  {
    t_line_comment.setArgument("comment", "Declaration of input parameters");
    t_line_comment.setArgument("whitespace", "\n");
    gen_content_input_param_decl += t_line_comment.processTemplate();

    for (const auto& input_param : umrf.getInputParameters())
    {
      std::string parameter_name = "in_param_" + input_param.getName();
      boost::replace_all(parameter_name, "::", "_");
      t_parameter_decl.setArgument("param_name_us", parameter_name);

      if (action_parameter::PARAMETER_MAP.find(input_param.getType()) != action_parameter::PARAMETER_MAP.end())
      {
        t_parameter_decl.setArgument("param_type_us", action_parameter::PARAMETER_MAP.at(input_param.getType()));
      }
      else
      {
        t_parameter_decl.setArgument("param_type_us", input_param.getType());
      }
      gen_content_input_param_decl += t_parameter_decl.processTemplate() + "\n";
    }
  }

  /*
   * Declare output parameters
   */
  std::string gen_content_output_param_decl;
  if (!umrf.getOutputParameters().empty())
  {
    t_line_comment.setArgument("comment", "Declaration of output parameters");
    t_line_comment.setArgument("whitespace", "\n");
    gen_content_output_param_decl += t_line_comment.processTemplate();

    for (const auto& output_param : umrf.getOutputParameters())
    {
      std::string parameter_name = "out_param_" + output_param.getName();
      boost::replace_all(parameter_name, "::", "_");
      t_parameter_decl.setArgument("param_name_us", parameter_name);

      if (action_parameter::PARAMETER_MAP.find(output_param.getType()) != action_parameter::PARAMETER_MAP.end())
      {
        t_parameter_decl.setArgument("param_type_us", action_parameter::PARAMETER_MAP.at(output_param.getType()));
      }
      else
      {
        t_parameter_decl.setArgument("param_type_us", output_param.getType());
      }
      gen_content_output_param_decl += t_parameter_decl.processTemplate() + "\n";
    }
  }

  /*
   * Put it all together and generate the whole class
   */
  t_class_base.setArgument("ta_class_name", ta_class_name);
  t_class_base.setArgument("ta_package_name", ta_package_name);
  t_class_base.setArgument("fn_get_input_parameters", t_get_input_params.processTemplate());
  t_class_base.setArgument("fn_set_output_parameters", t_set_output_params.processTemplate());
  t_class_base.setArgument("fn_execute_action", t_execute_action.processTemplate());
  t_class_base.setArgument("class_members", gen_content_input_param_decl + gen_content_output_param_decl);

  /*
   * Save the generated c++ content
   */
  tp::saveStrToFile(t_class_base.processTemplate(), ta_dst_path + "/src", ta_package_name, ".cpp");

  /*
   * Generate the temoto_action header
   */
  t_bridge_header.setArgument("ta_package_name", ta_package_name);
  std::string elif_blocks;
  for (const auto& elif_block : input_param_update_set)
  {
    elif_blocks += elif_block;
  }
  t_bridge_header.setArgument("update_parameters_content", elif_blocks);
  tp::saveStrToFile(t_bridge_header.processTemplate(), ta_dst_path + "/include/" + ta_package_name, "temoto_action", ".h");

}

void ActionPackageGenerator::generateGraph(const UmrfGraph& umrf_graph, const std::string& graphs_path)
{
  std::ofstream umrf_graph_json_file;
  umrf_graph_json_file.open (graphs_path + "/" + umrf_graph.getName() + ".umrfg.json");
  umrf_graph_json_file << umrf_json_converter::toUmrfGraphJsonStr(umrf_graph);
  umrf_graph_json_file.close();
}
}// temoto_action_assistant namespace