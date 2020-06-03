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

TabDialog::TabDialog(QWidget *parent)
: QDialog(parent)
{
  parameter_types_.insert({"test_param_1 (string)", ActionParameters::ParameterContainer("test_param_1", "string")});
  parameter_types_.insert({"test_param_2 (string)", ActionParameters::ParameterContainer("test_param_2", "string")});
  parameter_types_.insert({"test_param_3 (string)", ActionParameters::ParameterContainer("test_param_3", "string")});

  tab_widget_ = new QTabWidget;
  tab_widget_->addTab(new PredefinedParameterTab(parameter_types_, this), tr("Predefined"));
  tab_widget_->addTab(new CustomParameterTab(parameter_types_, this), tr("Custom"));

  button_box_ = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

  connect(button_box_, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(button_box_, &QDialogButtonBox::rejected, this, &QDialog::reject);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(tab_widget_);
  mainLayout->addWidget(button_box_);
  setLayout(mainLayout);

  setWindowTitle(tr("Add Parameter"));
}

const ActionParameters& TabDialog::getParameters() const
{
  return selected_parameters_;
}

PredefinedParameterTab::PredefinedParameterTab(TabDialog::ParameterTypes& parameter_types, QWidget *parent)
: QWidget(parent)
{
  QComboBox* parameter_type_field = new QComboBox(this);
  parameter_type_field->setMaximumWidth(400);
  parameter_type_field->setContextMenuPolicy(Qt::CustomContextMenu);

  // Add items to the combo box
  for (auto parameter_type : parameter_types)
  {
    parameter_type_field->addItem(parameter_type.first.c_str());
  }

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(parameter_type_field);
  mainLayout->addStretch(1);
  setLayout(mainLayout);
}

CustomParameterTab::CustomParameterTab(TabDialog::ParameterTypes& parameter_types, QWidget *parent)
: QWidget(parent)
{
  /*
   * TODO
   */
}