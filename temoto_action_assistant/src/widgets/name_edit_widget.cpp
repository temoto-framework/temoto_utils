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

#include "temoto_action_assistant/widgets/name_edit_widget.h"
#include "temoto_action_engine/action_parameter.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>

namespace temoto_action_assistant
{

// ******************************************************************************************
// Constructor
// ******************************************************************************************
NameEditWidget::NameEditWidget(QWidget *parent)
: QWidget(parent)
{
  // TODO: add a description element to the widget

  QVBoxLayout* parameter_editor_layout = new QVBoxLayout();
  // TODO: align it to the top

  QFormLayout* parameter_form_layout = new QFormLayout();
  parameter_form_layout->setContentsMargins(0, 15, 0, 15);
  parameter_editor_layout->addLayout(parameter_form_layout);

  /*
   * Create the name editing field
   */
  name_field_ = new QLineEdit(this);
  name_field_->setMaximumWidth(400);
  parameter_form_layout->addRow("Name:", name_field_);

  connect(name_field_, &QLineEdit::textChanged, this, &NameEditWidget::modifyName);

  this->setLayout(parameter_editor_layout);
}

// ******************************************************************************************
//
// ******************************************************************************************
void NameEditWidget::modifyName()
{
  std::string* name = boost::any_cast<std::string*>(tree_data_.payload_);
  *name = name_field_->text().toStdString();

  QString text = QString::fromStdString("Name: " + *name);
  tree_item_ptr_->setText(0, text);
}

// ******************************************************************************************
//
// ******************************************************************************************
void NameEditWidget::focusGiven(QTreeWidgetItem* tree_item_ptr)
{
  // Set the tree item pointer to active element
  tree_item_ptr_ = tree_item_ptr;
  tree_data_ = tree_item_ptr_->data(0, Qt::UserRole).value<UmrfTreeData>();
  std::string* name = boost::any_cast<std::string*>(tree_data_.payload_);

  /*
   * Update the word field
   */
  name_field_->setText(QString::fromStdString(*name));
}


} // temoto_action_assistant namespace
