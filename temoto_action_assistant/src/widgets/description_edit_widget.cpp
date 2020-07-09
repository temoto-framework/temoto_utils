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
#include <QMenu>

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
  parameter_editor_layout->setContentsMargins(0, 0, 0, 0);
  //parameter_editor_layout->addStretch(0);

  // QLabel* description_label = new QLabel();
  // description_label->setText("Description");
  // parameter_editor_layout->addWidget(description_label);

  /*
   * Create the name editing field
   */
  description_field_ = new QTextEdit();
  description_field_->setMaximumWidth(400);
  description_field_->setMaximumHeight(50);
  description_field_->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(description_field_, &QTextEdit::customContextMenuRequested, this, &DescriptionEditWidget::descriptionRightClick);


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
  if (umrf_)
  {
    umrf_->setDescription(description_field_->toPlainText().toStdString());
    Q_EMIT updated();
  }
}

// ******************************************************************************************
//
// ******************************************************************************************
void DescriptionEditWidget::setUmrf( std::shared_ptr<Umrf> umrf)
{
  umrf_ = umrf;
  description_field_->setText(QString::fromStdString(umrf_->getDescription()));
}

// ******************************************************************************************
//
// ******************************************************************************************
void DescriptionEditWidget::descriptionRightClick(const QPoint& pos)
{
  std::string selected_text = description_field_->textCursor().selectedText().toStdString();

  if (!selected_text.empty())
  {
    QMenu menu(this);
    QAction* add_action = new QAction(tr("&Annotate as Parameter"), this);
    connect(add_action, &QAction::triggered, this, &DescriptionEditWidget::parseSelctedText);
    menu.addAction(add_action);
    menu.exec(description_field_->mapToGlobal(pos));
  }
}

// ******************************************************************************************
//
// ******************************************************************************************
void DescriptionEditWidget::parseSelctedText()
{
  std::string selected_text = description_field_->textCursor().selectedText().toStdString();
  Q_EMIT textSelected(selected_text);
}
} // temoto_action_assistant namespace
