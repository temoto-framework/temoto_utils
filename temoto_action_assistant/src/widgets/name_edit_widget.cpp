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
NameEditWidget::NameEditWidget(QWidget *parent, std::shared_ptr<Umrf> umrf)
: QWidget(parent)
, umrf_(umrf)
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
  umrf_->setName(name_field_->text().toStdString());
}

// ******************************************************************************************
//
// ******************************************************************************************
void NameEditWidget::setUmrf( std::shared_ptr<Umrf> umrf)
{
  umrf_ = umrf;
  name_field_->setText(QString::fromStdString(umrf_->getName()));
}

} // temoto_action_assistant namespace
