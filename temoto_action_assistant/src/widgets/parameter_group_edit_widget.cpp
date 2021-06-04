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

#include "temoto_action_assistant/widgets/parameter_group_edit_widget.h"
#include "temoto_action_engine/action_parameter.h"
#include <boost/algorithm/string.hpp>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>

namespace temoto_action_assistant
{

// ******************************************************************************************
// Constructor
// ******************************************************************************************
ParameterGroupEditWidget::ParameterGroupEditWidget(QWidget *parent, std::shared_ptr<UmrfNode> umrf)
: QWidget(parent)
, umrf_(umrf)
{
  QFormLayout* parameter_form_layout = new QFormLayout();
  parameter_form_layout->setContentsMargins(0, 0, 0, 0);
 
  /*
   * Create the name editing field
   */
  parameter_group_name_field_ = new QLineEdit(this);
  parameter_group_name_field_->setMaximumWidth(400);
  parameter_form_layout->addRow("Name:", parameter_group_name_field_);

  connect(parameter_group_name_field_, &QLineEdit::textChanged, this, &ParameterGroupEditWidget::modifyName);

  this->setLayout(parameter_form_layout);
}

// ******************************************************************************************
//
// ******************************************************************************************
std::string ParameterGroupEditWidget::getGroupName(const std::string& group_name_full)
{
  std::vector<std::string> name_comp_vec;
  boost::split(name_comp_vec, group_name_full, boost::is_any_of("::"));
  return name_comp_vec.back();
}

// ******************************************************************************************
//
// ******************************************************************************************
void ParameterGroupEditWidget::modifyName()
{
  tree_data_ = tree_item_ptr_->data(0, Qt::UserRole).value<UmrfTreeData>();
  std::string parameter_group_name_full = boost::any_cast<std::string>(tree_data_.payload_);
  std::string parameter_group_name = getGroupName(parameter_group_name_full);
  std::string new_group_name = parameter_group_name_field_->text().toStdString();
  std::string new_group_name_full = parameter_group_name_full;

  if (new_group_name.empty() || new_group_name == parameter_group_name)
  {
    return;
  }

  ActionParameters* parameters;
  UmrfTreeData::ElementType type = tree_data_.type_;
  if (type == UmrfTreeData::PARAMETER_GROUP_IN)
  {
    parameters = &umrf_->getInputParametersNc();
  }
  else if (type == UmrfTreeData::PARAMETER_GROUP_OUT)
  {
    parameters = &umrf_->getOutputParametersNc();
  }
  else
  {
    return;
  }

  boost::replace_all(new_group_name_full, parameter_group_name, new_group_name);
  UmrfTreeData tree_item_group_pl = UmrfTreeData(type, new_group_name_full);
  tree_item_ptr_->setData(0, Qt::UserRole, QVariant::fromValue(tree_item_group_pl));

  for (auto& parameter : *parameters)
  {
    ActionParameters::ParameterContainer& param = const_cast<ActionParameters::ParameterContainer&>(parameter);
    std::string param_name_new = param.getName();
    boost::replace_all(param_name_new, parameter_group_name_full, new_group_name_full);
    param.setName(param_name_new);
  }

  QString text = QString::fromStdString(new_group_name);
  tree_item_ptr_->setText(0, text);
  //Q_EMIT refreshUmrfTree();
}

// ******************************************************************************************
//
// ******************************************************************************************
void ParameterGroupEditWidget::focusGiven(QTreeWidgetItem* tree_item_ptr)
{
  // Set the tree item pointer to active element
  tree_item_ptr_ = tree_item_ptr;
  tree_data_ = tree_item_ptr_->data(0, Qt::UserRole).value<UmrfTreeData>();
  std::string parameter_group_name_full = boost::any_cast<std::string>(tree_data_.payload_);

  // Get the name of the group wihout preceeding groups
  std::string parameter_group_name = getGroupName(parameter_group_name_full);

  // Update the name field
  parameter_group_name_field_->setText(QString::fromStdString(parameter_group_name));
}

// ******************************************************************************************
//
// ******************************************************************************************
void ParameterGroupEditWidget::setUmrf( std::shared_ptr<UmrfNode> umrf)
{
  umrf_ = umrf;
}

} // temoto_action_assistant namespace
