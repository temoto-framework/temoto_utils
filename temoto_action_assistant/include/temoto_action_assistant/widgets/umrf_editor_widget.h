/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2012, Willow Garage, Inc.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Willow Garage nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/

#ifndef TEMOTO_ACTION_ASSISTANT_UMRF_EDIT_WIDGET
#define TEMOTO_ACTION_ASSISTANT_UMRF_EDIT_WIDGET

// Qt
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedLayout>
#include <QString>
#include <QTreeWidget>

// SA
#ifndef Q_MOC_RUN
#endif

#include "temoto_action_assistant/widgets/header_widget.h"
#include "temoto_action_assistant/widgets/setup_screen_widget.h"  // a base class for screens in the setup assistant
#include "temoto_action_assistant/widgets/umrf_tree_data.h"
#include "temoto_action_assistant/widgets/parameter_group_edit_widget.h"
#include "temoto_action_assistant/widgets/parameter_edit_widget.h"
#include "temoto_action_assistant/widgets/name_edit_widget.h"
#include "temoto_action_assistant/widgets/effect_edit_widget.h"
#include "temoto_action_assistant/widgets/description_edit_widget.h"
#include "temoto_action_assistant/widgets/parameter_add_dialog_widget.h"
#include "temoto_action_assistant/widgets/umrf_graph_widget.h"
#include "temoto_action_assistant/threaded_action_indexer.h"
#include <memory>
#include "boost/any.hpp"
#include "temoto_action_engine/umrf_node.h"

namespace temoto_action_assistant
{

class UmrfEditorWidget : public SetupScreenWidget
{
Q_OBJECT

public:
  // ******************************************************************************************
  // Public Functions
  // ******************************************************************************************

  UmrfEditorWidget(QWidget* parent
  , std::string& umrf_graph_name
  , std::vector<std::shared_ptr<UmrfNode>>& umrfs
  , std::map<std::string, std::string>* custom_parameter_map
  , std::string umrf_parameters_path
  , std::shared_ptr<ThreadedActionIndexer> action_indexer);

  /// Recieved when this widget is chosen from the navigation menu
  virtual void focusGiven();

private Q_SLOTS:

  // ******************************************************************************************
  // Slot Event Functions
  // ******************************************************************************************

  /// Edit whatever element is selected in the tree view
  void editSelected();

  /// Creates a context based menu
  void createRightClickMenu(const QPoint& pos);

  void addParameterFromDescription(const std::string& text_to_annotate);

  void addParameter();

  void deleteParameter();

  void deleteParameterGroup();

  void subgroupParameter();

  void unSubgroupParameter();

  void setActiveUmrf(std::shared_ptr<UmrfNode> umrf);

  void hideUmrfEditor();

  void graphNameChanged();

  void annotateToAction(const QPoint& pos);

  void addAnnotatedAction();
  
  void addExistingAction(QAction *action);

private:
  // ******************************************************************************************
  // Variables
  // ******************************************************************************************

  /// Fonts
  const QFont top_level_font_;
  const QFont io_font_;
  const QFont type_font_;

  /// Contains all the configuration data for the semantic fram
  std::shared_ptr<UmrfNode> active_umrf_;
  std::string& umrf_graph_name_;
  std::vector<std::shared_ptr<UmrfNode>>& umrfs_;
  std::map<std::string, std::string>* custom_parameter_map_;
  int uniqueness_counter_;
  std::shared_ptr<ThreadedActionIndexer> action_indexer_;

  /// Variables for maintaining editing information
  UmrfTreeData active_tree_element_;
  QTreeWidgetItem* active_tree_item_;
  QLineEdit* graph_name_field_;
  QLineEdit* graph_description_field_;

  /// Widgets for editing the contents of the interfaces tree
  ParameterEditWidget* pew_;
  ParameterGroupEditWidget* pgew_;
  NameEditWidget* new_;
  DescriptionEditWidget* dew_;
  EffectEditWidget* eew_;
  TabDialog* td_;
  UmrfGraphWidget* ugw_;

  /// Main table for holding groups
  QTreeWidget* umrf_viz_tree_;
  QWidget* umrf_param_widget_;
  QStackedLayout* edit_screen_content_;
  QWidget* umrf_layout_widget_;
  QVBoxLayout* umrf_layout_;

  // ******************************************************************************************
  // Private Functions
  // ******************************************************************************************

  /// Populates the interfaces tree
  void populateInterfacesTree();

  ///
  void addParametersToTree(QTreeWidgetItem* upper_level_item_init, ActionParameters& action_parameters_in);

  /// Removes the data that interfaces tree element points to
  void removeData(UmrfTreeData &parent, UmrfTreeData &child);

  /// Builds a new tree based on updated action descriptor
  void refreshTree();

};

}  // namespace

#endif
