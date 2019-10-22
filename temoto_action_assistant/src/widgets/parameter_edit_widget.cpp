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

/* Author: Robert Valner */

#include "temoto_action_assistant/widgets/parameter_edit_widget.h"
#include "temoto_action_engine/action_parameter.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>

namespace temoto_action_assistant
{

// ******************************************************************************************
// Constructor
// ******************************************************************************************
ParameterEditWidget::ParameterEditWidget(QWidget *parent)
: QWidget(parent)
{
  // TODO: add a description element to the widget

  QVBoxLayout* parameter_editor_layout = new QVBoxLayout();
  // TODO: align it to the top

  QFormLayout* parameter_form_layout = new QFormLayout();
  parameter_form_layout->setContentsMargins(0, 15, 0, 15);
  parameter_editor_layout->addLayout(parameter_form_layout);

  /*
   * Create parameter type editing combobox
   */
  parameter_type_field_ = new QComboBox(this);
  parameter_type_field_->setMaximumWidth(400);
  parameter_form_layout->addRow("Type:", parameter_type_field_);

  connect(parameter_type_field_, SIGNAL(highlighted(QString)), this, SLOT(modifyType(QString)));

  // Add items to the combo box
  for (auto parameter_type : action_parameter::PARAMETER_MAP)
  {
    parameter_type_field_->addItem(parameter_type.first.c_str());
  }

  /*
   * Create the word editing field
   */
  parameter_name_field_ = new QLineEdit(this);
  parameter_name_field_->setMaximumWidth(400);
  parameter_form_layout->addRow("Name:", parameter_name_field_);

  connect(parameter_name_field_, &QLineEdit::textChanged, this, &ParameterEditWidget::modifyName);

  this->setLayout(parameter_editor_layout);
}

// ******************************************************************************************
//
// ******************************************************************************************
void ParameterEditWidget::modifyName()
{
  ActionParameters::ParameterContainer* parameter = boost::any_cast<ActionParameters::ParameterContainer*>(tree_data_.payload_);
  parameter->setNameKeepNamespace(parameter_name_field_->text().toStdString());

  QString text = QString::fromStdString(parameter->getNameNoNamespace() + " (" + parameter->getType() + ")");
  tree_item_ptr_->setText(0, text);
}

// ******************************************************************************************
//
// ******************************************************************************************
void ParameterEditWidget::modifyType(const QString &text)
{
  ActionParameters::ParameterContainer* parameter = boost::any_cast<ActionParameters::ParameterContainer*>(tree_data_.payload_);
  parameter->setType(text.toStdString());

  QString item_text = QString::fromStdString(parameter->getNameNoNamespace() + " (" + parameter->getType() + ")");
  tree_item_ptr_->setText(0, item_text);
}

// ******************************************************************************************
//
// ******************************************************************************************
void ParameterEditWidget::focusGiven(QTreeWidgetItem* tree_item_ptr)
{
  // Set the tree item pointer to active element
  tree_item_ptr_ = tree_item_ptr;
  tree_data_ = tree_item_ptr_->data(0, Qt::UserRole).value<UmrfTreeData>();
  ActionParameters::ParameterContainer* parameter = boost::any_cast<ActionParameters::ParameterContainer*>(tree_data_.payload_);

  /*
   * Update the word field
   */
  parameter_name_field_->setText(QString::fromStdString(parameter->getNameNoNamespace()));

  /*
   * Update the type field
   */
  int index = parameter_type_field_->findText(parameter->getType().c_str());
  if (index == -1)
  {
    // TODO: Throw an error
    return;
  }

  parameter_type_field_->setCurrentIndex(index);
}


} // temoto_action_assistant namespace
