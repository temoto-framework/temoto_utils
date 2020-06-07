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

/* Author: Robert Valner */

#include "temoto_action_assistant/widgets/parameter_add_dialog_widget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <boost/algorithm/string.hpp>

namespace temoto_action_assistant
{
TabDialog::TabDialog(QWidget *parent)
: QDialog(parent)
{
  ActionParameters ap1;
  ap1.setParameter(ActionParameters::ParameterContainer("group1::name1", "type1"));
  ap1.setParameter(ActionParameters::ParameterContainer("group1::name2", "type2"));

  ActionParameters ap2;
  ap2.setParameter(ActionParameters::ParameterContainer("parameterX", "type1"));

  addParameterType(ap1);
  addParameterType(ap2);
  
  tab_widget_ = new QTabWidget(this);
  PredefinedParameterTab* tab_predefined_params = new PredefinedParameterTab(parameter_types_, selected_parameters_name_, this);
  CustomParameterTab* tab_custom_params = new CustomParameterTab(parameter_types_, selected_parameters_name_, this);
  tab_widget_->addTab(tab_predefined_params, tr("Predefined"));
  tab_widget_->addTab(tab_custom_params, tr("Custom"));

  button_box_ = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  connect(button_box_, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(button_box_, &QDialogButtonBox::rejected, this, &TabDialog::dialogCancelled);

  // Initialize the umrf tree widget
  umrf_tree_widget_ = new UmrfTreeWidget(this);
  umrf_tree_widget_->addParametersToTree(parameter_types_.begin()->second);
  connect(tab_predefined_params->getParameterTypeField(), SIGNAL(highlighted(QString)), this, SLOT(parameterSelected(QString)));

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

ActionParameters TabDialog::getParameters() const
{
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
PredefinedParameterTab::PredefinedParameterTab(
  TabDialog::ParameterTypes& parameter_types,
  std::string& selected_parameters_name,
  QWidget *parent)
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
  selected_parameters_name_ = parameter_types.begin()->first;

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

// ******************************************************************************************
//
// ******************************************************************************************
CustomParameterTab::CustomParameterTab(TabDialog::ParameterTypes& parameter_types,
  std::string& selected_parameters_name,
  QWidget *parent)
: QWidget(parent)
, selected_parameters_name_(selected_parameters_name)
{
  /*
   * TODO
   */
}
} // temoto_action_assistant namespace