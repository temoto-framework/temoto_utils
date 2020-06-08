/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright 2020 TeMoto Telerobotics
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

#include "temoto_action_assistant/widgets/parameter_add_dialog_widget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>
#include <fstream>
#include "temoto_action_engine/umrf_json_converter.h"

namespace temoto_action_assistant
{
TabDialog::TabDialog(QWidget *parent, const std::string& umrf_parameters_path)
: QDialog(parent)
{
  parseUmrfParameterFiles(umrf_parameters_path);
  
  tab_widget_ = new QTabWidget(this);
  tab_predefined_params_ = new PredefinedParameterTab(parameter_types_, selected_parameters_name_, this);
  tab_custom_params_ = new CustomParameterTab(parameter_types_, selected_parameters_name_, this);
  tab_widget_->addTab(tab_predefined_params_, tr("Predefined"));
  tab_widget_->addTab(tab_custom_params_, tr("Custom"));

  button_box_ = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  connect(button_box_, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(button_box_, &QDialogButtonBox::rejected, this, &TabDialog::dialogCancelled);

  // Initialize the umrf tree widget
  umrf_tree_widget_ = new UmrfTreeWidget(this);
  if (!parameter_types_.empty())
  {
    umrf_tree_widget_->addParametersToTree(parameter_types_.begin()->second);
  }
  connect(tab_predefined_params_->getParameterTypeField(), SIGNAL(highlighted(QString)), this, SLOT(parameterSelected(QString)));

  QHBoxLayout *tools_layout = new QHBoxLayout;
  tools_layout->setAlignment(Qt::AlignBaseline);
  tools_layout->addWidget(tab_widget_);
  tools_layout->addWidget(umrf_tree_widget_);

  QVBoxLayout *main_layout = new QVBoxLayout;
  main_layout->addLayout(tools_layout);
  main_layout->addWidget(button_box_);
  setLayout(main_layout);

  setWindowTitle(tr("Add Parameter"));
}

void TabDialog::parseUmrfParameterFiles(const std::string& umrf_parameters_path)
{
  for (auto& itr: boost::filesystem::directory_iterator(umrf_parameters_path))
  {
    try
    {
      // if its a file and matches the desc file name, process the file
      if ( !boost::filesystem::is_regular_file(itr) 
      || (itr.path().string().find("param.umrf.json") == std::string::npos) )
      {
        continue;
      }
      std::ifstream umrf_param_fs(itr.path().string());
      std::string umrf_json_str;
      umrf_json_str.assign(std::istreambuf_iterator<char>(umrf_param_fs), std::istreambuf_iterator<char>());
      rapidjson::Document json_doc;
      json_doc.Parse(umrf_json_str.c_str());

      if (json_doc.HasParseError())
      {
        std::cout << "The provided JSON string contains syntax errors." << std::endl;
        continue;
      }
      ActionParameters::Parameters umrf_parameters = umrf_json_converter::parseParameters(json_doc, "");
      addParameterType(umrf_parameters);
    }
    catch (std::exception& e)
    {
      // Rethrow the exception
      //throw CREATE_ERROR(temoto_core::error::Code::FIND_TASK_FAIL, e.what());
    }

    catch(...)
    {
      // Rethrow the exception
      //throw CREATE_ERROR(temoto_core::error::Code::UNHANDLED_EXCEPTION, "Received an unhandled exception");
    }
  }
}

bool TabDialog::addParameterType(const ActionParameters& action_parameters_in)
{
  if (action_parameters_in.empty())
  {
    return false;
  }
  
  // Set the name
  std::vector<std::string> name_tokens;
  boost::split(name_tokens, action_parameters_in.begin()->getName(), boost::is_any_of("::"));
  std::string parameter_name = name_tokens.front();

  // Add the type of the parameter to the back of the name
  if (action_parameters_in.getParameterCount() > 1)
  {
    parameter_name += " (compound)";
  }
  else
  {
    parameter_name += " (" + action_parameters_in.begin()->getType() + ")";
  }
  return parameter_types_.insert({parameter_name, action_parameters_in}).second;
}

ActionParameters TabDialog::getParameters()
{
  tab_predefined_params_->refreshSelectedParameter();
  exec();

  if (!selected_parameters_name_.empty())
  {
    return parameter_types_.find(selected_parameters_name_)->second;
  }
  else
  {
    return ActionParameters();
  }
}

void TabDialog::parameterSelected(const QString &text)
{
  umrf_tree_widget_->addParametersToTree(parameter_types_[text.toStdString()]);
}

void TabDialog::dialogCancelled()
{
  selected_parameters_name_ = "";
  reject();
}

// ******************************************************************************************
//
// ******************************************************************************************
PredefinedParameterTab::PredefinedParameterTab(TabDialog::ParameterTypes& parameter_types
, std::string& selected_parameters_name
, QWidget *parent)
: QWidget(parent)
, selected_parameters_name_(selected_parameters_name)
{
  parameter_type_field_ = new QComboBox(this);
  parameter_type_field_->setMaximumWidth(400);
  parameter_type_field_->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(parameter_type_field_, SIGNAL(highlighted(QString)), this, SLOT(setSelectedParameters(QString)));

  // Add items to the combo box
  for (auto parameter_type : parameter_types)
  {
    parameter_type_field_->addItem(parameter_type.first.c_str());
  }

  // Initialize the default selection
  if (!parameter_types.empty())
  {
    selected_parameters_name_ = parameter_types.begin()->first;
  }

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(parameter_type_field_);
  mainLayout->addStretch(1);
  setLayout(mainLayout);
}

QComboBox* PredefinedParameterTab::getParameterTypeField()
{
  return parameter_type_field_;
}

void PredefinedParameterTab::setSelectedParameters(const QString &text)
{
  selected_parameters_name_ = text.toStdString();
}

void PredefinedParameterTab::refreshSelectedParameter()
{
  selected_parameters_name_ = parameter_type_field_->currentText().toStdString();
}

// ******************************************************************************************
//
// ******************************************************************************************
CustomParameterTab::CustomParameterTab(TabDialog::ParameterTypes& parameter_types
, std::string& selected_parameters_name
, QWidget *parent)
: QWidget(parent)
, selected_parameters_name_(selected_parameters_name)
{
  /*
   * TODO
   */
}
} // temoto_action_assistant namespace