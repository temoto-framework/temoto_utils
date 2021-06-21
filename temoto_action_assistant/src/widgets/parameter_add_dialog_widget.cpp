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
#include "temoto_action_engine/umrf_json_converter.h"
#include "temoto_action_engine/umrf_graph_fs.h"

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
      
      std::string umrf_param_json_str = temoto_action_engine::readFromFile(itr.path().string());
      ActionParameters::Parameters umrf_parameters = umrf_json_converter::fromUmrfParametersJsonStr(umrf_param_json_str);
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

int countWords(const char* str)
{
   bool in_spaces = true;
   int numWords = 0;

   while (*str != '\0')
   {
      if (std::isspace(*str))
      {
         in_spaces = true;
      }
      else if (in_spaces)
      {
         numWords++;
         in_spaces = false;
      }

      ++str;
   }

   return numWords;
}

ActionParameters TabDialog::getParameters(const std::string& text_to_annotate)
{
  text_to_annotate_ = text_to_annotate;

  if (!text_to_annotate_.empty())
  {
    selected_parameters_name_ = "";
    umrf_tree_widget_->clear();
    int word_count = countWords(text_to_annotate_.c_str());
    tab_predefined_params_->repopulateParameterField(parameter_types_, word_count);
  }
  else
  {
    tab_predefined_params_->refreshSelectedParameter();
    tab_predefined_params_->repopulateParameterField(parameter_types_);
  }
  
  exec();
  text_to_annotate_ = "";

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
  repopulateParameterField(parameter_types);

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

void PredefinedParameterTab::repopulateParameterField(TabDialog::ParameterTypes& parameter_types, const int& max_nr_of_subparams)
{
  parameter_type_field_->clear();
  for (auto parameter_type : parameter_types)
  {
    if ((parameter_type.second.getParameterCount() > max_nr_of_subparams) && (max_nr_of_subparams != 0))
    {
      continue;
    }
    parameter_type_field_->addItem(parameter_type.first.c_str());
  }

  if (!selected_parameters_name_.empty())
  {
    int index = parameter_type_field_->findText(selected_parameters_name_.c_str());
    if (index == -1)
    {
      // TODO: Throw an error
      return;
    }
    parameter_type_field_->setCurrentIndex(index);
    Q_EMIT parameter_type_field_->highlighted(selected_parameters_name_.c_str());
  }
  else
  {
    parameter_type_field_->setCurrentIndex(0);
    Q_EMIT parameter_type_field_->highlighted(parameter_type_field_->itemText(0));
  }
  
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