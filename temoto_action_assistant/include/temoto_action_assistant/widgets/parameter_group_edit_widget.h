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

#ifndef TEMOTO_ACTION_ASSISTANT_PARAMETER_GROUP_EDIT_WIDGET
#define TEMOTO_ACTION_ASSISTANT_PARAMETER_GROUP_EDIT_WIDGET

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QTreeWidget>

#ifndef Q_MOC_RUN
#endif

#include "temoto_action_assistant/widgets/umrf_tree_data.h"
#include "temoto_action_engine/umrf.h"
#include <memory>

namespace temoto_action_assistant
{
class ParameterGroupEditWidget : public QWidget
{
  Q_OBJECT

public:
  // ******************************************************************************************
  // Public Functions
  // ******************************************************************************************

  /// Constructor
  ParameterGroupEditWidget(QWidget* parent, std::shared_ptr<Umrf> umrf);

  /// Focus given
  void focusGiven(QTreeWidgetItem* tree_item_ptr);

  void setUmrf(std::shared_ptr<Umrf> umrf);

  // ******************************************************************************************
  // Qt Components
  // ******************************************************************************************

  QLabel* title_;  // specify the title from the parent widget
  QLineEdit* parameter_group_name_field_;

private Q_SLOTS:

  // ******************************************************************************************
  // Slot Event Functions
  // ******************************************************************************************

  /// Modifies the word variable
  void modifyName();

Q_SIGNALS:

  // ******************************************************************************************
  // Emitted Signals
  // ******************************************************************************************
  void refreshUmrfTree();

private:
  std::string getGroupName(const std::string& group_name_full);

  /// Points to the active element of the interface tree
  UmrfTreeData tree_data_;
  QTreeWidgetItem* tree_item_ptr_;
  std::shared_ptr<Umrf> umrf_;
};
}

#endif
