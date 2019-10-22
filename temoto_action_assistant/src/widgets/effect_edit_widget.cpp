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

#include "temoto_action_assistant/widgets/effect_edit_widget.h"
#include "temoto_action_engine/umrf.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>

namespace temoto_action_assistant
{

// ******************************************************************************************
// Constructor
// ******************************************************************************************
EffectEditWidget::EffectEditWidget(QWidget *parent)
: QWidget(parent)
{
  // TODO: add a description element to the widget

  QVBoxLayout* effect_editor_layout = new QVBoxLayout();
  // TODO: align it to the top

  QFormLayout* effect_form_layout = new QFormLayout();
  effect_form_layout->setContentsMargins(0, 15, 0, 15);
  effect_editor_layout->addLayout(effect_form_layout);

  /*
   * Create parameter type editing combobox
   */
  effect_type_field_ = new QComboBox(this);
  effect_type_field_->setMaximumWidth(400);
  effect_form_layout->addRow("Effect:", effect_type_field_);

  connect(effect_type_field_, SIGNAL(highlighted(QString)), this, SLOT(modifyEffect(QString)));

  // Add items to the combo box
  for (auto effect_type : action_effect::EFFECT_LIST)
  {
    effect_type_field_->addItem(effect_type.c_str());
  }

  this->setLayout(effect_editor_layout);
}

// ******************************************************************************************
//
// ******************************************************************************************
void EffectEditWidget::modifyEffect(const QString &text)
{
  std::string* effect = boost::any_cast<std::string*>(tree_data_.payload_);
  *effect = text.toStdString();

  QString effect_text = QString::fromStdString("Effect: " + *effect);
  tree_item_ptr_->setText(0, effect_text);
}

// ******************************************************************************************
//
// ******************************************************************************************
void EffectEditWidget::focusGiven(QTreeWidgetItem* tree_item_ptr)
{
  // Set the tree item pointer to active element
  tree_item_ptr_ = tree_item_ptr;
  tree_data_ = tree_item_ptr_->data(0, Qt::UserRole).value<UmrfTreeData>();
  std::string* effect = boost::any_cast<std::string*>(tree_data_.payload_);

  /*
   * Update the effect field
   */
  int index = effect_type_field_->findText(effect->c_str());
  if (index == -1)
  {
    // TODO: Throw an error
    return;
  }

  effect_type_field_->setCurrentIndex(index);
}


} // temoto_action_assistant namespace
