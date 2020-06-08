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

#ifndef TEMOTO_ACTION_ASSISTANT_PARAMETER_ADD_DIALOG_WIDGET
#define TEMOTO_ACTION_ASSISTANT_PARAMETER_ADD_DIALOG_WIDGET

#include "temoto_action_engine/action_parameters.h"
#include <QDialog>
#include <QTabWidget>
#include <QDialogButtonBox>
#include <QComboBox>
#include "temoto_action_assistant/widgets/umrf_tree_widget.h"

#ifndef Q_MOC_RUN
#endif
namespace temoto_action_assistant
{

// Forward declare the tabs
class PredefinedParameterTab;
class CustomParameterTab;

class TabDialog : public QDialog
{
  Q_OBJECT

public:
  typedef std::map<std::string, ActionParameters> ParameterTypes;

  TabDialog(QWidget *parent, const std::string& umrf_parameters_path);
  ActionParameters getParameters();

private:
  bool addParameterType(const ActionParameters& action_parameters_in);

  void parseUmrfParameterFiles(const std::string& umrf_parameters_path);

  QTabWidget *tab_widget_;
  QDialogButtonBox *button_box_;
  ParameterTypes parameter_types_;
  std::string selected_parameters_name_;
  UmrfTreeWidget* umrf_tree_widget_;

  /// Tabs
  PredefinedParameterTab* tab_predefined_params_;
  CustomParameterTab* tab_custom_params_;

private Q_SLOTS:
  /// 
  void parameterSelected(const QString &text);
  void dialogCancelled();
};

/**
 * @brief Tab widget that maintains predefined parameters
 * 
 */
class PredefinedParameterTab : public QWidget
{
  Q_OBJECT

public:
  PredefinedParameterTab( TabDialog::ParameterTypes& parameter_types
  , std::string& selected_parameters_name
  , QWidget *parent = nullptr);

  QComboBox* getParameterTypeField();
  
  void refreshSelectedParameter();

private:
  QComboBox* parameter_type_field_;
  std::string& selected_parameters_name_;

private Q_SLOTS:
  void setSelectedParameters(const QString &text);
};

/**
 * @brief Tab widget that hepls to create custom parameters
 * 
 */
class CustomParameterTab : public QWidget
{
  Q_OBJECT

public:
  CustomParameterTab( TabDialog::ParameterTypes& parameter_types
  , std::string& selected_parameters_name
  , QWidget *parent = nullptr);

private:
  std::string& selected_parameters_name_;

private Q_SLOTS:

};
} // temoto_action_assistant namespace
#endif