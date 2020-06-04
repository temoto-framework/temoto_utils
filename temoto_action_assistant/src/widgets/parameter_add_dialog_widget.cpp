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
#include <QComboBox>
#include <boost/algorithm/string.hpp>

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
  
  tab_widget_ = new QTabWidget;
  tab_widget_->addTab(new PredefinedParameterTab(parameter_types_, selected_parameters_name_, this), tr("Predefined"));
  tab_widget_->addTab(new CustomParameterTab(parameter_types_, selected_parameters_name_, this), tr("Custom"));

  button_box_ = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

  connect(button_box_, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(button_box_, &QDialogButtonBox::rejected, this, &QDialog::reject);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(tab_widget_);
  mainLayout->addWidget(button_box_);
  setLayout(mainLayout);

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

const ActionParameters& TabDialog::getParameters() const
{
  return parameter_types_.find(selected_parameters_name_)->second;
}

PredefinedParameterTab::PredefinedParameterTab(
  TabDialog::ParameterTypes& parameter_types,
  std::string& selected_parameters_name,
  QWidget *parent)
: QWidget(parent)
, selected_parameters_name_(selected_parameters_name)
{
  QComboBox* parameter_type_field = new QComboBox(this);
  parameter_type_field->setMaximumWidth(400);
  parameter_type_field->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(parameter_type_field, SIGNAL(highlighted(QString)), this, SLOT(setSelectedParameters(QString)));

  // Add items to the combo box
  for (auto parameter_type : parameter_types)
  {
    parameter_type_field->addItem(parameter_type.first.c_str());
  }

  // Initialize the default selection
  selected_parameters_name_ = parameter_types.begin()->first;

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(parameter_type_field);
  mainLayout->addStretch(1);
  setLayout(mainLayout);
}

void PredefinedParameterTab::setSelectedParameters(const QString &text)
{
  selected_parameters_name_ = text.toStdString();
}

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