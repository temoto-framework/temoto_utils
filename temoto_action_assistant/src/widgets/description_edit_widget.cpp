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

#include "temoto_action_assistant/widgets/description_edit_widget.h"
#include "temoto_action_engine/action_parameter.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>

namespace temoto_action_assistant
{

// ******************************************************************************************
// Constructor
// ******************************************************************************************
DescriptionEditWidget::DescriptionEditWidget(QWidget *parent, std::shared_ptr<Umrf> umrf)
: QWidget(parent)
, umrf_(umrf)
{
  // TODO: add a description element to the widget

  QVBoxLayout* parameter_editor_layout = new QVBoxLayout();
  parameter_editor_layout->addStretch(0);

  //QFormLayout* parameter_form_layout = new QFormLayout();
  //parameter_form_layout->setContentsMargins(0, 15, 0, 15);
  //parameter_editor_layout->addLayout(parameter_form_layout);

  QLabel* description_label = new QLabel();
  description_label->setText("Description");
  parameter_editor_layout->addWidget(description_label);

  /*
   * Create the name editing field
   */
  description_field_ = new QTextEdit();
  description_field_->setMaximumWidth(400);
  description_field_->setMaximumHeight(50);
  parameter_editor_layout->addWidget(description_field_);

  //parameter_form_layout->addRow("Description:", description_field_);
  connect(description_field_, &QTextEdit::textChanged, this, &DescriptionEditWidget::modifyDescription);

  this->setLayout(parameter_editor_layout);
}

// ******************************************************************************************
//
// ******************************************************************************************
void DescriptionEditWidget::modifyDescription()
{
  umrf_->setDescription(description_field_->toPlainText().toStdString());
}

// ******************************************************************************************
//
// ******************************************************************************************
// void DescriptionEditWidget::focusGiven(QTreeWidgetItem* tree_item_ptr)
// {
//   // Set the tree item pointer to active element
//   tree_item_ptr_ = tree_item_ptr;
//   tree_data_ = tree_item_ptr_->data(0, Qt::UserRole).value<UmrfTreeData>();
//   std::string* name = boost::any_cast<std::string*>(tree_data_.payload_);

//   /*
//    * Update the word field
//    */
//   description_field_->setText(QString::fromStdString(*name));
// }
} // temoto_action_assistant namespace
