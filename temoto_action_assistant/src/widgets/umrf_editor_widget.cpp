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

/* Author: Dave Coleman */

#include "temoto_action_assistant/widgets/umrf_editor_widget.h"
#include "temoto_action_assistant/widgets/parameter_add_dialog_widget.h"
#include <QFormLayout>
#include <QMessageBox>
#include <QAction>
#include <QMenu>
#include <QStyle>
#include <iostream>
#include <boost/algorithm/string.hpp>

namespace temoto_action_assistant
{
// ******************************************************************************************
// Constructor
// ******************************************************************************************
UmrfEditorWidget::UmrfEditorWidget(QWidget* parent
, std::shared_ptr<Umrf> umrf
, std::map<std::string, std::string>* custom_parameter_map)
: SetupScreenWidget(parent),
  umrf_(umrf),
  custom_parameter_map_(custom_parameter_map),
  uniqueness_counter_(0),
  top_level_font_(QFont(QFont().defaultFamily(), 11, QFont::Bold)),
  io_font_(QFont(QFont().defaultFamily(), 10, QFont::Bold)),
  type_font_(QFont(QFont().defaultFamily(), 10, QFont::Normal, QFont::StyleItalic))

{
  // TODO: add a description element to the widget

  // Basic widget container
  QVBoxLayout* layout = new QVBoxLayout();
  QHBoxLayout* layout_e_t = new QHBoxLayout();

  /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
   *                                Create content for the top layer
   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

  // Add header
  HeaderWidget* header = new HeaderWidget("UMRF Editor", "Edit UMRF structures", this);
  layout->addWidget(header);

  /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
   *                               Create content for the edit screen
   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
  QVBoxLayout* edit_screen_top_layout = new QVBoxLayout();
  edit_screen_content_ = new QStackedLayout();
  layout_e_t->addLayout(edit_screen_top_layout);
  edit_screen_top_layout->addLayout(edit_screen_content_);

  /*
   * Dummy editor page
   */
  QWidget* dummy_widget = new QWidget();
  edit_screen_content_->addWidget(dummy_widget);

  /*
   * Name editor page
   */
  new_ = new NameEditWidget(parent);
  edit_screen_content_->addWidget(new_);

  /*
   * Effect editor page
   */
  eew_ = new EffectEditWidget(parent);
  edit_screen_content_->addWidget(eew_);

  /*
   * Parameter Group editor page
   */
  pgew_ = new ParameterGroupEditWidget(parent, umrf_);
  edit_screen_content_->addWidget(pgew_);

  /*
   * Parameter editor page
   */
  pew_ = new ParameterEditWidget(parent, custom_parameter_map_);
  edit_screen_content_->addWidget(pew_);

  /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
   *                           Create content for the interfaces tree
   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
  umrf_viz_tree_widget_ = createContentsWidget();
  layout_e_t->addWidget(umrf_viz_tree_widget_);

  /*
   * Add the edtior/tree layout to the top layout
   */
  layout->addLayout(layout_e_t);
  layout->setAlignment(Qt::AlignTop);
  this->setLayout(layout);

  populateInterfacesTree();
  umrf_viz_tree_->expandAll();
}

// ******************************************************************************************
// Create the main tree view widget
// ******************************************************************************************
QWidget* UmrfEditorWidget::createContentsWidget()
{
  // Main widget
  QWidget* content_widget = new QWidget(this);

  // Basic widget container
  QVBoxLayout* layout = new QVBoxLayout(this);

  // Add the lexical unit edit field
  QFormLayout* form_layout = new QFormLayout();
  layout->addLayout(form_layout);


  // Tree Box ----------------------------------------------------------------------
  umrf_viz_tree_ = new QTreeWidget(this);
  umrf_viz_tree_->setHeaderLabel("UMRF");
  umrf_viz_tree_->setContextMenuPolicy(Qt::CustomContextMenu);

  connect(umrf_viz_tree_, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(editSelected()));
  connect(umrf_viz_tree_, &QTreeWidget::customContextMenuRequested, this, &UmrfEditorWidget::createRightClickMenu);
  layout->addWidget(umrf_viz_tree_);

  // Bottom Controls -------------------------------------------------------------
  QHBoxLayout* controls_layout = new QHBoxLayout();

  // Expand/Contract controls
  QLabel* expand_controls = new QLabel(this);
  expand_controls->setText("<a href='expand'>Expand All</a> <a href='contract'>Collapse All</a>");
  //connect(expand_controls, SIGNAL(linkActivated(const QString)), this, SLOT(alterTree(const QString)));
  controls_layout->addWidget(expand_controls);

  // Spacer
  QWidget* spacer = new QWidget(this);
  spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  controls_layout->addWidget(spacer);

  // Add Controls to layout
  layout->addLayout(controls_layout);

  // Set layout
  content_widget->setLayout(layout);

  return content_widget;
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
void UmrfEditorWidget::addParameter()
{
  UmrfTreeData::ElementType type = active_tree_element_.type_;
  std::string group_name = boost::any_cast<std::string>(active_tree_element_.payload_);
  // if (type == UmrfTreeData::INPUT_PARAMETERS)
  // {
  //   umrf_->getInputParametersNc().setParameter(ActionParameters::ParameterContainer(
  //     "RENAME_PARAM_" + std::to_string(uniqueness_counter_++)));
  // }
  // else if (type == UmrfTreeData::PARAMETER_GROUP_IN)
  // {
  //   umrf_->getInputParametersNc().setParameter(ActionParameters::ParameterContainer(
  //     group_name + "::" + "RENAME_PARAM_" + std::to_string(uniqueness_counter_++)));
  // }
  // else if (type == UmrfTreeData::OUTPUT_PARAMETERS)
  // {
  //   umrf_->getOutputParametersNc().setParameter(ActionParameters::ParameterContainer(
  //     "RENAME_PARAM_" + std::to_string(uniqueness_counter_++)));
  // }
  // else if(type == UmrfTreeData::PARAMETER_GROUP_OUT)
  // {
  //   umrf_->getOutputParametersNc().setParameter(ActionParameters::ParameterContainer(
  //     group_name + "::" + "RENAME_PARAM_" + std::to_string(uniqueness_counter_++)));
  // }

  TabDialog td(this);
  td.exec();

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
    umrf_->getInputParametersNc().removeParameter(parameter->getName());
  }
  else
  {
    umrf_->getOutputParametersNc().removeParameter(parameter->getName());
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
    param_names = umrf_->getInputParameters().getParamNames();
    parameters = &umrf_->getInputParametersNc();
  }
  else
  {
    param_names = umrf_->getOutputParameters().getParamNames();
    parameters = &umrf_->getOutputParametersNc();
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

  if (type == UmrfTreeData::NAME)
  {
    new_->focusGiven(active_tree_item_);
    edit_screen_content_->setCurrentIndex(1);
  }
  if (type == UmrfTreeData::EFFECT)
  {
    eew_->focusGiven(active_tree_item_);
    edit_screen_content_->setCurrentIndex(2);
  }
  else if (type == UmrfTreeData::INPUT_PARAMETERS || type == UmrfTreeData::OUTPUT_PARAMETERS)
  {
    edit_screen_content_->setCurrentIndex(0);
  }
  else if (type == UmrfTreeData::PARAMETER_GROUP_IN || type == UmrfTreeData::PARAMETER_GROUP_OUT)
  {
    pgew_->focusGiven(active_tree_item_);
    edit_screen_content_->setCurrentIndex(3);
  }
  else if (type == UmrfTreeData::PARAMETER_IN || type == UmrfTreeData::PARAMETER_OUT)
  {
    pew_->focusGiven(active_tree_item_);      // Set up the parameter editor
    edit_screen_content_->setCurrentIndex(4); // Show the parameter editor
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
  std::cout << "**************************************" << std::endl;
  for (const auto& param : umrf_->getInputParameters())
  {
    std::cout << " -in-  * " << param.getName() << std::endl;
  }
  for (const auto& param : umrf_->getOutputParameters())
  {
    std::cout << " -out- * " << param.getName() << std::endl;
  }

  /*
   * Add name item
   */
  {
    std::string name_str =  std::string("Name: ") + umrf_->getName();
    QString name_qstr = QString::fromStdString(name_str);

    QTreeWidgetItem* tree_item_name = new QTreeWidgetItem(umrf_viz_tree_);
    tree_item_name->setText(0, name_qstr);
    tree_item_name->setFont(0, top_level_font_);
    UmrfTreeData tree_item_name_pl = UmrfTreeData(
                                    UmrfTreeData::NAME,
                                    boost::any_cast<std::string*>(&umrf_->getNameNc()));
    tree_item_name->setData(0, Qt::UserRole, QVariant::fromValue(tree_item_name_pl));
    umrf_viz_tree_->addTopLevelItem(tree_item_name);
  }

  /*
   * Add effect item
   */
  {
    std::string effect_str =  std::string("Effect: ") + umrf_->getEffect();
    QString name_qstr = QString::fromStdString(effect_str);

    QTreeWidgetItem* tree_item_name = new QTreeWidgetItem(umrf_viz_tree_);
    tree_item_name->setText(0, name_qstr);
    tree_item_name->setFont(0, top_level_font_);
    UmrfTreeData tree_item_name_pl = UmrfTreeData(
                                    UmrfTreeData::EFFECT,
                                    boost::any_cast<std::string*>(&umrf_->getEffectNc()));
    tree_item_name->setData(0, Qt::UserRole, QVariant::fromValue(tree_item_name_pl));
    umrf_viz_tree_->addTopLevelItem(tree_item_name);
  }

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
    addParametersToTree(tree_item_input_params, umrf_->getInputParametersNc());
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
    addParametersToTree(tree_item_output_params, umrf_->getOutputParametersNc());
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
        tree_item_param->setText(0, QString::fromStdString(param.getNameNoNamespace() + " (" + param.getType() + ")"));
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

} // temoto_action_assistant namespace
