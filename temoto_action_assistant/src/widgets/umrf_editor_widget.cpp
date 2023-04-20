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

#include "temoto_action_assistant/widgets/umrf_editor_widget.h"
#include <QFormLayout>
#include <QMessageBox>
#include <QAction>
#include <QMenu>
#include <QStyle>
#include <QScrollArea>
#include <QScrollBar>
#include <iostream>
#include <boost/algorithm/string.hpp>

namespace temoto_action_assistant
{
// ******************************************************************************************
// Constructor
// ******************************************************************************************
UmrfEditorWidget::UmrfEditorWidget(QWidget* parent
, std::string& umrf_graph_name
, std::vector<std::shared_ptr<UmrfNode>>& umrfs
, std::map<std::string, std::string>* custom_parameter_map
, std::string umrf_parameters_path
, std::shared_ptr<ThreadedActionIndexer> action_indexer)
: SetupScreenWidget(parent),
  umrf_graph_name_(umrf_graph_name),
  umrfs_(umrfs),
  custom_parameter_map_(custom_parameter_map),
  action_indexer_(action_indexer),
  uniqueness_counter_(0),
  top_level_font_(QFont(QFont().defaultFamily(), 11, QFont::Bold, QFont::StyleItalic)),
  io_font_(QFont(QFont().defaultFamily(), 10, QFont::Bold)),
  type_font_(QFont(QFont().defaultFamily(), 10, QFont::Normal, QFont::StyleItalic))

{
  // Basic widget container
  QVBoxLayout* layout = new QVBoxLayout();
  QHBoxLayout* layout_e_t = new QHBoxLayout();
  layout_e_t->setAlignment(Qt::AlignTop);

  /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
   *                                Create content for the top layer
   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

  // Add header
  HeaderWidget* header = new HeaderWidget("UMRF Editor", "", this);
  layout->addWidget(header);

  /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
   *                               Create content for the edit screen
   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
  QVBoxLayout* umrf_graph_layout = new QVBoxLayout();
  umrf_graph_layout->setAlignment(Qt::AlignTop);
  QFormLayout* graph_form_layout = new QFormLayout();
  graph_form_layout->setContentsMargins(0, 0, 0, 0);

  // TODO: The graph name and description fields should be defined in UmrfGraphWidget class
  graph_name_field_ = new QLineEdit();
  if (umrf_graph_name_.empty())
  {
    graph_name_field_->setPlaceholderText("umrf_graph_0");
  }
  else
  {
    graph_name_field_->setText(umrf_graph_name_.c_str());
  }
  
  graph_description_field_ = new QLineEdit();
  graph_description_field_->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(graph_description_field_, &QLineEdit::customContextMenuRequested, this, &UmrfEditorWidget::annotateToAction);

  graph_form_layout->addRow("Graph name:", graph_name_field_);
  graph_form_layout->addRow("Graph description:", graph_description_field_);
  connect(graph_name_field_, &QLineEdit::textChanged, this, &UmrfEditorWidget::graphNameChanged);

  QScrollArea* scroll_area = new QScrollArea();
  //scroll_area->setStyleSheet("background-color:white;");
  ugw_ = new UmrfGraphWidget(scroll_area, umrfs_, action_indexer_);
  connect(ugw_, &UmrfGraphWidget::activeUmrfChanged, this, &UmrfEditorWidget::setActiveUmrf);
  connect(ugw_, &UmrfGraphWidget::noUmrfSelected, this, &UmrfEditorWidget::hideUmrfEditor);
  scroll_area->setWidget(ugw_);

  umrf_graph_layout->addLayout(graph_form_layout);
  umrf_graph_layout->addWidget(scroll_area);

  layout_e_t->addLayout(umrf_graph_layout);
  layout_e_t->setAlignment(umrf_graph_layout, Qt::AlignTop);

  // Add vertical line
  QFrame* v_line = new QFrame();
  v_line->setContentsMargins(10, 0, 0, 0);
  v_line->setFrameShape(QFrame::VLine);
  v_line->setFrameShadow(QFrame::Sunken);
  layout_e_t->addWidget(v_line);

  /*
   * Parameter adding dialog widget
   */
  td_ = new TabDialog(this, umrf_parameters_path); 

  /*
   * Create and populate the UMRF editor layout
   */
  QVBoxLayout* umrf_editor_with_spacer_layout = new QVBoxLayout();
  umrf_editor_with_spacer_layout->setAlignment(Qt::AlignTop);
  layout_e_t->addLayout(umrf_editor_with_spacer_layout);
  layout_e_t->setAlignment(umrf_editor_with_spacer_layout, Qt::AlignTop);

  umrf_layout_widget_ = new QWidget();
  umrf_layout_ = new QVBoxLayout(umrf_layout_widget_);
  umrf_editor_with_spacer_layout->addWidget(umrf_layout_widget_);
  umrf_editor_with_spacer_layout->setAlignment(umrf_layout_widget_, Qt::AlignTop);
  umrf_layout_widget_->setVisible(false);

  // Form layout for description, name and effect fields
  QFormLayout* dne_form_layout = new QFormLayout();
  dne_form_layout->setContentsMargins(0, 0, 0, 0);
  umrf_layout_->addLayout(dne_form_layout);

  // Description editor widget
  dew_ = new DescriptionEditWidget(parent, active_umrf_);
  connect(dew_, &DescriptionEditWidget::textSelected, this, &UmrfEditorWidget::addParameterFromDescription);
  connect(dew_, &DescriptionEditWidget::updated, ugw_, &UmrfGraphWidget::refreshGraph);
  dne_form_layout->addRow("Description", dew_);
  //umrf_layout_->setAlignment(dew_, Qt::AlignTop);

  // Name editor widget
  new_ = new NameEditWidget(parent, active_umrf_);
  connect(new_, &NameEditWidget::updated, ugw_, &UmrfGraphWidget::refreshGraph);
  dne_form_layout->addRow("Name", new_);
  //umrf_layout_->setAlignment(new_, Qt::AlignTop);

  // Effect editor page
  eew_ = new EffectEditWidget(parent, active_umrf_);
  dne_form_layout->addRow("Effect", eew_);
  //umrf_layout_->setAlignment(eew_, Qt::AlignTop);

  // Parameter visualizer widget
  umrf_viz_tree_ = new QTreeWidget();
  umrf_viz_tree_->setHeaderLabel("UMRF Parameters");
  umrf_viz_tree_->setContextMenuPolicy(Qt::CustomContextMenu);
  umrf_viz_tree_->setMinimumHeight(250);
  umrf_viz_tree_->horizontalScrollBar()->setEnabled(true);
  connect(umrf_viz_tree_, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(editSelected()));
  connect(umrf_viz_tree_, &QTreeWidget::customContextMenuRequested, this, &UmrfEditorWidget::createRightClickMenu);
  umrf_layout_->addWidget(umrf_viz_tree_);
  umrf_layout_->setAlignment(umrf_viz_tree_, Qt::AlignTop);

  // Add horizontal line
  QFrame* h_line = new QFrame();
  h_line->setFrameShape(QFrame::HLine);
  h_line->setFrameShadow(QFrame::Sunken);
  umrf_layout_->addWidget(h_line);

  /*
   * Create and populate the parameter editor layout
   */  
  edit_screen_content_ = new QStackedLayout();
  edit_screen_content_->setContentsMargins(0, 0, 0, 0);

  // Dummy editor page
  QWidget* dummy_widget = new QWidget();
  edit_screen_content_->addWidget(dummy_widget);

  // Parameter Group editor page
  pgew_ = new ParameterGroupEditWidget(parent, active_umrf_);
  edit_screen_content_->addWidget(pgew_);

  // Parameter editor page
  pew_ = new ParameterEditWidget(parent, custom_parameter_map_);
  edit_screen_content_->addWidget(pew_);

  umrf_layout_->addLayout(edit_screen_content_);
  umrf_layout_->setAlignment(edit_screen_content_, Qt::AlignTop);

  // Add a spacer for maintainig the width of the umrf editor screen when hidden
  QWidget* spacer = new QWidget(this);
  spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  spacer->setMinimumWidth(200);
  spacer->setMaximumHeight(1);
  umrf_editor_with_spacer_layout->addWidget(spacer);
  umrf_editor_with_spacer_layout->setAlignment(spacer, Qt::AlignTop);

  /*
   * Add the edtior/tree layout to the top layout
   */
  layout->addLayout(layout_e_t);
  this->setLayout(layout);
}

// ******************************************************************************************
// 
// ******************************************************************************************
void UmrfEditorWidget::createRightClickMenu(const QPoint& pos)
{
  active_tree_item_ = umrf_viz_tree_->itemAt(pos);

  // Check that something was actually selected
  if (active_tree_item_ == NULL)
  {
    return;
  }

  // Get the user custom properties of the currently selected row
  active_tree_element_ = active_tree_item_->data(0, Qt::UserRole).value<UmrfTreeData>();
  
  QMenu menu(this);
  UmrfTreeData::ElementType type = active_tree_element_.type_;
  if (type == UmrfTreeData::NAME)
  {
     /* Do nothing */
  }
  else if (type == UmrfTreeData::INPUT_PARAMETERS || type == UmrfTreeData::OUTPUT_PARAMETERS)
  {
    /*
     * Create menu for input or output parameters
     */
    QAction* add_parameter_action = new QAction(tr("&ADD Parameter"), this);
    add_parameter_action->setIcon(this->style()->standardIcon(this->style()->SP_DialogApplyButton));
    connect(add_parameter_action, SIGNAL(triggered()), this, SLOT(addParameter()));
    menu.addAction(add_parameter_action);
  }
  else if (type == UmrfTreeData::PARAMETER_GROUP_IN || type == UmrfTreeData::PARAMETER_GROUP_OUT)
  {
    /*
     * Create menu for parameter group
     */
    QAction* add_parameter_action = new QAction(tr("&ADD Parameter To This Group"), this);
    add_parameter_action->setIcon(this->style()->standardIcon(this->style()->SP_DialogApplyButton));
    connect(add_parameter_action, SIGNAL(triggered()), this, SLOT(addParameter()));
    menu.addAction(add_parameter_action);
    
    QAction* remove_group_action = new QAction(tr("&DELETE This Group"), this);
    remove_group_action->setIcon(this->style()->standardIcon(this->style()->SP_DialogCancelButton));
    connect(remove_group_action, SIGNAL(triggered()), this, SLOT(deleteParameterGroup()));
    menu.addAction(remove_group_action);
  }
  else if (type == UmrfTreeData::PARAMETER_IN || type == UmrfTreeData::PARAMETER_OUT)
  {
    /*
     * Create menu for parameter
     */
    QAction* create_subgroup_action = new QAction(tr("&SUB-GROUP This Parameter"), this);
    create_subgroup_action->setIcon(this->style()->standardIcon(this->style()->SP_DialogApplyButton));
    connect(create_subgroup_action, SIGNAL(triggered()), this, SLOT(subgroupParameter()));
    menu.addAction(create_subgroup_action);

    QAction* remove_subgroup_action = new QAction(tr("&UN-SUB-GROUP This Parameter"), this);
    remove_subgroup_action->setIcon(this->style()->standardIcon(this->style()->SP_DialogCancelButton));
    connect(remove_subgroup_action, SIGNAL(triggered()), this, SLOT(unSubgroupParameter()));
    menu.addAction(remove_subgroup_action);

    QAction* remove_parameter_action = new QAction(tr("&DELETE This Parameter"), this);
    remove_parameter_action->setIcon(this->style()->standardIcon(this->style()->SP_DialogCancelButton));
    connect(remove_parameter_action, SIGNAL(triggered()), this, SLOT(deleteParameter()));
    menu.addAction(remove_parameter_action);
  }
  else
  {
    QMessageBox::critical(this, "Error Loading", "An internal error has occured while loading.");
  }

  // Create the menu where the cursor is
  QPoint pt(pos);
  menu.exec(umrf_viz_tree_->mapToGlobal(pos));
}

// ******************************************************************************************
// 
// ******************************************************************************************
void UmrfEditorWidget::addParameterFromDescription(const std::string& text_to_annotate)
{
  ActionParameters aps = td_->getParameters(text_to_annotate);
  for (const auto& ap : aps)
  {
    active_umrf_->getInputParametersNc().setParameter(ap);
  }
  refreshTree();
}

// ******************************************************************************************
// 
// ******************************************************************************************
void UmrfEditorWidget::addParameter()
{
  UmrfTreeData::ElementType type = active_tree_element_.type_;
  std::string group_name = boost::any_cast<std::string>(active_tree_element_.payload_);

  //td_->exec();
  ActionParameters aps = td_->getParameters();

  if (aps.empty())
  {
    return;
  }

  if (type == UmrfTreeData::INPUT_PARAMETERS)
  {
    for (const auto& ap : aps)
    {
      active_umrf_->getInputParametersNc().setParameter(ap);
    }
  }
  else if (type == UmrfTreeData::PARAMETER_GROUP_IN)
  {
    for (const auto& ap : aps)
    {
      ActionParameters::ParameterContainer new_ap = ap;
      new_ap.setName(group_name + "::" + ap.getName());
      active_umrf_->getInputParametersNc().setParameter(new_ap);
    }
  }
  else if (type == UmrfTreeData::OUTPUT_PARAMETERS)
  {
    for (const auto& ap : aps)
    {
      active_umrf_->getOutputParametersNc().setParameter(ap);
    }
  }
  else if(type == UmrfTreeData::PARAMETER_GROUP_OUT)
  {
    for (const auto& ap : aps)
    {
      ActionParameters::ParameterContainer new_ap = ap;
      new_ap.setName(group_name + "::" + ap.getName());
      active_umrf_->getOutputParametersNc().setParameter(new_ap);
    }
  }

  refreshTree();
}

// ******************************************************************************************
// 
// ******************************************************************************************
void UmrfEditorWidget::deleteParameter()
{
  UmrfTreeData::ElementType type = active_tree_element_.type_;
  if (type != UmrfTreeData::PARAMETER_IN && type != UmrfTreeData::PARAMETER_OUT)
  {
    return;
  }

  ActionParameters::ParameterContainer* parameter = boost::any_cast<ActionParameters::ParameterContainer*>(active_tree_element_.payload_);
  if (type == UmrfTreeData::PARAMETER_IN)
  {
    active_umrf_->getInputParametersNc().removeParameter(parameter->getName());
  }
  else
  {
    active_umrf_->getOutputParametersNc().removeParameter(parameter->getName());
  }
  edit_screen_content_->setCurrentIndex(0);
  refreshTree();
}

// ******************************************************************************************
// 
// ******************************************************************************************
void UmrfEditorWidget::deleteParameterGroup()
{
  UmrfTreeData::ElementType type = active_tree_element_.type_;
  if (type != UmrfTreeData::PARAMETER_GROUP_IN && type != UmrfTreeData::PARAMETER_GROUP_OUT)
  {
    return;
  }

  std::string group_name = boost::any_cast<std::string>(active_tree_element_.payload_);
  std::set<std::string> param_names;
  ActionParameters* parameters;

  if (type == UmrfTreeData::PARAMETER_GROUP_IN)
  {
    param_names = active_umrf_->getInputParameters().getParamNames();
    parameters = &active_umrf_->getInputParametersNc();
  }
  else
  {
    param_names = active_umrf_->getOutputParameters().getParamNames();
    parameters = &active_umrf_->getOutputParametersNc();
  }

  for (const auto& param_name : param_names)
  {
    if(param_name.find(group_name) != std::string::npos)
    {
      parameters->removeParameter(param_name);
    }
  }
  edit_screen_content_->setCurrentIndex(0);
  refreshTree();
}

// ******************************************************************************************
// 
// ******************************************************************************************
void UmrfEditorWidget::subgroupParameter()
{
  UmrfTreeData::ElementType type = active_tree_element_.type_;
  if (type != UmrfTreeData::PARAMETER_IN && type != UmrfTreeData::PARAMETER_OUT)
  {
    return;
  }
  ActionParameters::ParameterContainer* parameter = boost::any_cast<ActionParameters::ParameterContainer*>(active_tree_element_.payload_);
  parameter->setNameKeepNamespace("RENAME_PARAMETER_GROUP_" + std::to_string(uniqueness_counter_++) + "::" 
    + parameter->getNameNoNamespace());
  refreshTree();
}

// ******************************************************************************************
// 
// ******************************************************************************************
void UmrfEditorWidget::unSubgroupParameter()
{
  UmrfTreeData::ElementType type = active_tree_element_.type_;
  if (type != UmrfTreeData::PARAMETER_IN && type != UmrfTreeData::PARAMETER_OUT)
  {
    return;
  }
  ActionParameters::ParameterContainer* parameter = boost::any_cast<ActionParameters::ParameterContainer*>(active_tree_element_.payload_);
  parameter->removeNamespaceLevel();
  refreshTree();
}

// ******************************************************************************************
// Edit whatever element is selected in the tree view
// ******************************************************************************************
void UmrfEditorWidget::editSelected()
{
  active_tree_item_ = umrf_viz_tree_->currentItem();

  // Check that something was actually selected
  if (active_tree_item_ == NULL)
  {
    return;
  }

  // Get the user custom properties of the currently selected row
  active_tree_element_ = active_tree_item_->data(0, Qt::UserRole).value<UmrfTreeData>();
  UmrfTreeData::ElementType type = active_tree_element_.type_;

  if (type == UmrfTreeData::INPUT_PARAMETERS || type == UmrfTreeData::OUTPUT_PARAMETERS)
  {
    edit_screen_content_->setCurrentIndex(0);
  }
  else if (type == UmrfTreeData::PARAMETER_GROUP_IN || type == UmrfTreeData::PARAMETER_GROUP_OUT)
  {
    pgew_->focusGiven(active_tree_item_);
    edit_screen_content_->setCurrentIndex(1);
  }
  else if (type == UmrfTreeData::PARAMETER_IN || type == UmrfTreeData::PARAMETER_OUT)
  {
    pew_->focusGiven(active_tree_item_);      // Set up the parameter editor
    edit_screen_content_->setCurrentIndex(2); // Show the parameter editor
  }
  else
  {
    QMessageBox::critical(this, "Error", "An internal error has occured ...");
  }
}

// ******************************************************************************************
//
// ******************************************************************************************
void UmrfEditorWidget::populateInterfacesTree()
{
  /*
   * Add input parameters
   */
  {
    QTreeWidgetItem* tree_item_input_params = new QTreeWidgetItem(umrf_viz_tree_);
    tree_item_input_params->setText(0, QString::fromStdString("Input Parameters"));
    tree_item_input_params->setFont(0, top_level_font_);
    UmrfTreeData tree_item_input_params_pl = UmrfTreeData(
                                            UmrfTreeData::INPUT_PARAMETERS,
                                            boost::any_cast<std::string>(std::string("")));
    tree_item_input_params->setData(0, Qt::UserRole, QVariant::fromValue(tree_item_input_params_pl));
    umrf_viz_tree_->addTopLevelItem(tree_item_input_params);                                      

    // Add the parameters
    addParametersToTree(tree_item_input_params, active_umrf_->getInputParametersNc());
  }

  /*
   * Add output parameters
   */
  {
    QTreeWidgetItem* tree_item_output_params = new QTreeWidgetItem(umrf_viz_tree_);
    tree_item_output_params->setText(0, QString::fromStdString("Output Parameters"));
    tree_item_output_params->setFont(0, top_level_font_);
    UmrfTreeData tree_item_output_params_pl = UmrfTreeData(
                                            UmrfTreeData::OUTPUT_PARAMETERS,
                                            boost::any_cast<std::string>(std::string("")));
    tree_item_output_params->setData(0, Qt::UserRole, QVariant::fromValue(tree_item_output_params_pl));
    umrf_viz_tree_->addTopLevelItem(tree_item_output_params);                                      

    // Add the parameters
    addParametersToTree(tree_item_output_params, active_umrf_->getOutputParametersNc());
  }
}

// ******************************************************************************************
//
// ******************************************************************************************
void UmrfEditorWidget::addParametersToTree( QTreeWidgetItem* upper_level_item_init
                                          , ActionParameters& action_parameters_in)
{
  bool is_input;
  UmrfTreeData::ElementType type = upper_level_item_init->data(0, Qt::UserRole).value<UmrfTreeData>().type_;
  if (type == UmrfTreeData::ElementType::INPUT_PARAMETERS)
  {
    is_input = true;
  }
  else if (type == UmrfTreeData::ElementType::OUTPUT_PARAMETERS)
  {
    is_input = false;
  }
  else
  {
    return;
  }
  
  std::map<std::string, QTreeWidgetItem*> param_group_map;
  for (auto& param : action_parameters_in)
  {
    // Split the name into substrings to identify the parameter groups (Refer to UMRF parameter format definiton)
    std::vector<std::string> param_name_comps;
    boost::split(param_name_comps, param.getName(), boost::is_any_of("::"));
    
    // Remove empty strings
    for (auto cmp_it=param_name_comps.begin(); cmp_it!=param_name_comps.end(); /* empty */)
    {
      if (!cmp_it->empty())
      {
        cmp_it++;
        continue;
      }
      param_name_comps.erase(cmp_it);
    }

    // Define few helper variables which are going to be used when looping over the param name components
    std::string param_full_name;
    QTreeWidgetItem* upper_level_item = upper_level_item_init;

    for (unsigned int i=0; i<param_name_comps.size(); i++)
    {
      param_full_name += param_name_comps.at(i);

      // If this is the parameter, not a group name then add new parameter to the tree
      if (param_name_comps.size()-i == 1)
      {  
        QTreeWidgetItem* tree_item_param = new QTreeWidgetItem(upper_level_item);
        if (param.getExample().empty())
        {
          tree_item_param->setText(0, QString::fromStdString(param.getNameNoNamespace() 
          + " (" + param.getType() + ")"));
        }
        else
        {
          tree_item_param->setText(0, QString::fromStdString(param.getNameNoNamespace() 
          + " (" + param.getType() + "), e.g. '" + param.getExample() + "'"));
        }
        
        tree_item_param->setFont(0, type_font_);
        UmrfTreeData tree_item_param_pl = UmrfTreeData(
          is_input ? UmrfTreeData::PARAMETER_IN : UmrfTreeData::PARAMETER_OUT,
          boost::any_cast<ActionParameters::ParameterContainer*>(
            &const_cast<ActionParameters::ParameterContainer&>(param)));
        tree_item_param->setData(0, Qt::UserRole, QVariant::fromValue(tree_item_param_pl));
        upper_level_item->addChild(tree_item_param);
      }
      else
      {
        // Check if this parameter or parameter group exists in the tree_item_map
        if (param_group_map.find(param_full_name) == param_group_map.end())
        {
          QTreeWidgetItem* tree_item_group = new QTreeWidgetItem(upper_level_item);
          tree_item_group->setText(0, QString::fromStdString(param_name_comps.at(i)));
          tree_item_group->setFont(0, io_font_);
          UmrfTreeData tree_item_group_pl = UmrfTreeData(
            is_input ? UmrfTreeData::PARAMETER_GROUP_IN : UmrfTreeData::PARAMETER_GROUP_OUT,
            boost::any_cast<std::string>(std::string(param_full_name)));
          tree_item_group->setData(0, Qt::UserRole, QVariant::fromValue(tree_item_group_pl));
          upper_level_item->addChild(tree_item_group);
          param_group_map[param_full_name] = tree_item_group;
        }

        // Set this group item as the new upper level item
        upper_level_item = param_group_map[param_full_name];
      }

      // Add param group separator symbols (Refer to UMRF parameter format definiton)
      param_full_name += "::";
    }
  }
}

// ******************************************************************************************
//
// ******************************************************************************************
void UmrfEditorWidget::refreshTree()
{
  umrf_viz_tree_->clear();      // Clear the tree
  populateInterfacesTree();       // Create new contents for the tree
  umrf_viz_tree_->expandAll();  // Expand the tree. TODO: the user might not like it
}

// ******************************************************************************************
//
// ******************************************************************************************
void UmrfEditorWidget::focusGiven()
{

}

// ******************************************************************************************
//
// ******************************************************************************************
void UmrfEditorWidget::setActiveUmrf(std::shared_ptr<UmrfNode> umrf)
{
  active_umrf_ = umrf;
  edit_screen_content_->setCurrentIndex(0);
  dew_->setUmrf(active_umrf_);
  new_->setUmrf(active_umrf_);
  eew_->setUmrf(active_umrf_);
  pgew_->setUmrf(active_umrf_);

  refreshTree();
  umrf_layout_widget_->setVisible(true);
}

// ******************************************************************************************
//
// ******************************************************************************************
void UmrfEditorWidget::hideUmrfEditor()
{
  umrf_layout_widget_->setVisible(false);
}

// ******************************************************************************************
//
// ******************************************************************************************
void UmrfEditorWidget::graphNameChanged()
{
  std::string new_name = graph_name_field_->text().toStdString();
  if (!new_name.empty())
  {
    umrf_graph_name_ = new_name;
  }
}

// ******************************************************************************************
//
// ******************************************************************************************
void UmrfEditorWidget::annotateToAction(const QPoint& pos)
{
  std::string selected_text = graph_description_field_->selectedText().toStdString();
  if (!selected_text.empty())
  {
    QMenu menu(this);
    QAction* add_action = new QAction(tr("&Annotate as Action"), this);
    connect(add_action, &QAction::triggered, this, &UmrfEditorWidget::addAnnotatedAction);
    menu.addAction(add_action);

    if (action_indexer_->getActionCount() > 0)
    {
      QMenu* sub_menu = menu.addMenu("Annotate as Existing Action");
      connect(sub_menu, &QMenu::triggered, this, &UmrfEditorWidget::addExistingAction);
      for (const auto& umrf_name : action_indexer_->getUmrfNames())
      {
        QAction* action = new QAction(umrf_name.c_str(), this);
        sub_menu->addAction(action);
      }
    }

    menu.exec(graph_description_field_->mapToGlobal(pos));
  }
}

// ******************************************************************************************
//
// ******************************************************************************************
void UmrfEditorWidget::addAnnotatedAction()
{
  UmrfNode new_umrf;
  new_umrf.setEffect("synchronous");
  new_umrf.setDescription(graph_description_field_->selectedText().toStdString());
  ugw_->addUmrf(new_umrf);
}

void UmrfEditorWidget::addExistingAction(QAction *action)
{
  UmrfNode new_umrf = action_indexer_->getUmrf(action->text().toStdString());
  new_umrf.setDescription(graph_description_field_->selectedText().toStdString());
  new_umrf.setName(new_umrf.getPackageName());
  ugw_->addUmrf(new_umrf);
}

} // temoto_action_assistant namespace
