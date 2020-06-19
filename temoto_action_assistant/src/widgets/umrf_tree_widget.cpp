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

#include "temoto_action_assistant/widgets/umrf_tree_widget.h"
#include <QMenu>
#include <QStyle>
#include <QVBoxLayout>
#include <iostream>
#include <boost/algorithm/string.hpp>

namespace temoto_action_assistant
{
// ******************************************************************************************
// Constructor
// ******************************************************************************************
UmrfTreeWidget::UmrfTreeWidget(QWidget* parent)
: QWidget(parent),
  uniqueness_counter_(0),
  top_level_font_(QFont(QFont().defaultFamily(), 11, QFont::Bold)),
  io_font_(QFont(QFont().defaultFamily(), 10, QFont::Bold)),
  type_font_(QFont(QFont().defaultFamily(), 10, QFont::Normal, QFont::StyleItalic))
{

  // Create the UMRF tree
  umrf_viz_tree_ = new QTreeWidget(this);
  umrf_viz_tree_->setHeaderLabel("Selected Parameter");
  umrf_viz_tree_->setContextMenuPolicy(Qt::CustomContextMenu);

  QVBoxLayout* layout = new QVBoxLayout;
  layout->addWidget(umrf_viz_tree_);
  layout->setAlignment(Qt::AlignTop);
  layout->addStretch(1);
  setLayout(layout);

  //populateInterfacesTree();
  // umrf_viz_tree_->expandAll();
}

// ******************************************************************************************
//
// ******************************************************************************************
void UmrfTreeWidget::populateInterfacesTree()
{
  /*
   * Add input parameters
   */
  {
    QTreeWidgetItem* tree_item_input_params = new QTreeWidgetItem(umrf_viz_tree_);
    tree_item_input_params->setText(0, QString::fromStdString("Parameters"));
    tree_item_input_params->setFont(0, top_level_font_);
    UmrfTreeData tree_item_input_params_pl = UmrfTreeData(
                                            UmrfTreeData::INPUT_PARAMETERS,
                                            boost::any_cast<std::string>(std::string("")));
    umrf_viz_tree_->addTopLevelItem(tree_item_input_params);                                      
    // Add the parameters
    addParametersToTree(umrf_->getInputParametersNc(), tree_item_input_params);
  }
}

// ******************************************************************************************
//
// ******************************************************************************************
void UmrfTreeWidget::addParametersToTree( ActionParameters& action_parameters_in, QTreeWidgetItem* upper_level_item_in)
{
  umrf_viz_tree_->clear();
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
    QTreeWidgetItem* upper_level_item = upper_level_item_in;
    
    for (unsigned int i=0; i<param_name_comps.size(); i++)
    {
      param_full_name += param_name_comps.at(i);

      // If this is the parameter, not a group name then add new parameter to the tree
      if (param_name_comps.size()-i == 1)
      {  
        QTreeWidgetItem* tree_item_param = new QTreeWidgetItem;
        tree_item_param->setText(0, QString::fromStdString(param.getNameNoNamespace() + " (" + param.getType() + ")"));
        tree_item_param->setFont(0, type_font_);
        UmrfTreeData tree_item_param_pl = UmrfTreeData(
          UmrfTreeData::PARAMETER_IN,
          boost::any_cast<ActionParameters::ParameterContainer*>(
            &const_cast<ActionParameters::ParameterContainer&>(param)));
        tree_item_param->setData(0, Qt::UserRole, QVariant::fromValue(tree_item_param_pl));

        if (upper_level_item)
        {
          upper_level_item->addChild(tree_item_param);
        }
        else
        {
          umrf_viz_tree_->addTopLevelItem(tree_item_param);
        }
      }
      else
      {
        // Check if this parameter or parameter group exists in the tree_item_map
        if (param_group_map.find(param_full_name) == param_group_map.end())
        {
          QTreeWidgetItem* tree_item_group = new QTreeWidgetItem;
          tree_item_group->setText(0, QString::fromStdString(param_name_comps.at(i)));
          tree_item_group->setFont(0, io_font_);
          UmrfTreeData tree_item_group_pl = UmrfTreeData(
            UmrfTreeData::PARAMETER_GROUP_IN,
            boost::any_cast<std::string>(std::string(param_full_name)));
          tree_item_group->setData(0, Qt::UserRole, QVariant::fromValue(tree_item_group_pl));
          param_group_map[param_full_name] = tree_item_group;

          if (upper_level_item_in)
          {
            upper_level_item->addChild(tree_item_group);
          }
          else
          {
            umrf_viz_tree_->addTopLevelItem(tree_item_group);
          }
        }

        // Set this group item as the new upper level item
        upper_level_item = param_group_map[param_full_name];
      }

      // Add param group separator symbols (Refer to UMRF parameter format definiton)
      param_full_name += "::";
    }
  }

  umrf_viz_tree_->expandAll();
}

// ******************************************************************************************
//
// ******************************************************************************************
void UmrfTreeWidget::refreshTree()
{
  umrf_viz_tree_->clear();      // Clear the tree
  populateInterfacesTree();       // Create new contents for the tree
  umrf_viz_tree_->expandAll();  // Expand the tree. TODO: the user might not like it
}

void UmrfTreeWidget::clear()
{
  umrf_viz_tree_->clear();
}

} // temoto_action_assistant namespace
