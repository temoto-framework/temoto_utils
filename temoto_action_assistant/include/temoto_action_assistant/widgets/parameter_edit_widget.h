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

#ifndef TEMOTO_ACTION_ASSISTANT_PARAMETER_EDIT_WIDGET
#define TEMOTO_ACTION_ASSISTANT_PARAMETER_EDIT_WIDGET

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QTreeWidget>

#ifndef Q_MOC_RUN
#endif

#include "temoto_action_assistant/widgets/umrf_tree_data.h"
#include "temoto_action_engine/umrf_node.h"

namespace temoto_action_assistant
{
class ParameterEditWidget : public QWidget
{
  Q_OBJECT

public:
  // ******************************************************************************************
  // Public Functions
  // ******************************************************************************************

  /// Constructor
  ParameterEditWidget(QWidget* parent, std::map<std::string, std::string>* custom_parameter_map);

  /// Focus given
  void focusGiven(QTreeWidgetItem* tree_item_ptr);

  /// Creates a context based menu
  void createRightClickMenu(const QPoint& pos);

  // ******************************************************************************************
  // Qt Components
  // ******************************************************************************************

  QLabel* title_;  // specify the title from the parent widget
  QLineEdit* parameter_name_field_;
  QLineEdit* parameter_example_field_;
  QComboBox* parameter_type_field_;

private Q_SLOTS:

  // ******************************************************************************************
  // Slot Event Functions
  // ******************************************************************************************

  /// Modifies the name variable
  void modifyName();

  /// Modifies the name variable
  void modifyExample();

  /// Modify the type variable
  void modifyType(const QString &text);

  void addTypeDialog();

Q_SIGNALS:

  // ******************************************************************************************
  // Emitted Signals
  // ******************************************************************************************
  void refreshUmrfTree();

private:
  // ******************************************************************************************
  // Variables
  // ******************************************************************************************

  /// Points to the active element of the interface tree
  UmrfTreeData tree_data_;
  std::map<std::string, std::string>* custom_parameter_map_;
  QTreeWidgetItem* tree_item_ptr_;

  // ******************************************************************************************
  // Private Functions
  // ******************************************************************************************
  void refreshTreeItemText(const ActionParameters::ParameterContainer* const parameter);

};
}

#endif
