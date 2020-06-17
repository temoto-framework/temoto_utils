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

#ifndef TEMOTO_ACTION_ASSISTANT_INTERFACE_EDIT_WIDGET
#define TEMOTO_ACTION_ASSISTANT_INTERFACE_EDIT_WIDGET

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QTreeWidget>

#ifndef Q_MOC_RUN
#endif

#include "interface_tree_data.h"
#include "temoto_action_assistant/semantic_frame.h"

namespace temoto_action_assistant
{
class InterfaceEditWidget : public QWidget
{
  Q_OBJECT

public:
  // ******************************************************************************************
  // Public Functions
  // ******************************************************************************************

  /// Constructor
  InterfaceEditWidget(QWidget* parent);

  /// Focus given
  void focusGiven(QTreeWidgetItem* tree_item_ptr);

  // ******************************************************************************************
  // Qt Components
  // ******************************************************************************************

  QLabel* title_;  // specify the title from the parent widget
  QComboBox* interface_type_field_;

private Q_SLOTS:

  // ******************************************************************************************
  // Slot Event Functions
  // ******************************************************************************************

  /// Modify the type variable
  void modifyType(const QString &text);

Q_SIGNALS:

  // ******************************************************************************************
  // Emitted Signals
  // ******************************************************************************************

private:
  // ******************************************************************************************
  // Variables
  // ******************************************************************************************

  /// Points to the active element of the interface tree
  InterfaceTreeData tree_data_;
  QTreeWidgetItem* tree_item_ptr_;


  // ******************************************************************************************
  // Private Functions
  // ******************************************************************************************


};
}

#endif
