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

#ifndef TEMOTO_ACTION_ASSISTANT_UMRF_TREE_WIDGET_H
#define TEMOTO_ACTION_ASSISTANT_UMRF_TREE_WIDGET_H

#include <QWidget>
#include <QString>
#include <QTreeWidget>

#ifndef Q_MOC_RUN
#endif

#include "temoto_action_assistant/widgets/umrf_tree_data.h"
#include "temoto_action_engine/umrf.h"
#include <memory>
#include "boost/any.hpp"

namespace temoto_action_assistant
{

class UmrfTreeWidget : public QWidget
{
  Q_OBJECT

public:
  UmrfTreeWidget(QWidget* parent);

  /// 
  void addParametersToTree(ActionParameters& action_parameters_in, QTreeWidgetItem* upper_level_item_init = NULL);

  /// Populates the interfaces tree
  void populateInterfacesTree();

  /// Builds a new tree based on updated action descriptor
  void refreshTree();

private:
  /// Fonts
  const QFont top_level_font_;
  const QFont io_font_;
  const QFont type_font_;

  /// Contains all the configuration data for the semantic frame
  std::shared_ptr<Umrf> umrf_;
  std::map<std::string, std::string>* custom_parameter_map_;
  int uniqueness_counter_;

  /// Variables for maintaining editing information
  UmrfTreeData active_tree_element_;
  QTreeWidgetItem* active_tree_item_;

  /// Main table for holding groups
  QTreeWidget* umrf_viz_tree_;
};

}  // namespace

#endif
