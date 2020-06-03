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

#ifndef TEMOTO_ACTION_ASSISTANT_PARAMETER_ADD_DIALOG_WIDGET
#define TEMOTO_ACTION_ASSISTANT_PARAMETER_ADD_DIALOG_WIDGET

#include "temoto_action_engine/action_parameters.h"
#include <QDialog>
#include <QTabWidget>
#include <QDialogButtonBox>

class TabDialog : public QDialog
{
  Q_OBJECT

public:
  typedef std::map<std::string, ActionParameters::ParameterContainer> ParameterTypes;

  explicit TabDialog(QWidget *parent = nullptr);
  const ActionParameters& getParameters() const;

private:
  QTabWidget *tab_widget_;
  QDialogButtonBox *button_box_;
  ParameterTypes parameter_types_;
  ActionParameters selected_parameters_;
};

class PredefinedParameterTab : public QWidget
{
  Q_OBJECT

public:
  explicit PredefinedParameterTab(TabDialog::ParameterTypes& parameter_types, QWidget *parent = nullptr);
};

class CustomParameterTab : public QWidget
{
  Q_OBJECT

public:
  explicit CustomParameterTab(TabDialog::ParameterTypes& parameter_types, QWidget *parent = nullptr);
};

#endif