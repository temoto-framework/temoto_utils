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

#include "temoto_action_assistant/widgets/effect_edit_widget.h"
#include "temoto_action_engine/umrf_node.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>

namespace temoto_action_assistant
{

// ******************************************************************************************
// Constructor
// ******************************************************************************************
EffectEditWidget::EffectEditWidget(QWidget *parent, std::shared_ptr<UmrfNode> umrf)
: QWidget(parent)
, umrf_(umrf)
{
  QVBoxLayout* effect_form_layout = new QVBoxLayout();
  effect_form_layout->setContentsMargins(0, 0, 0, 0);

  /*
   * Create parameter type editing combobox
   */
  effect_type_field_ = new QComboBox(this);
  effect_type_field_->setMaximumWidth(400);
  effect_form_layout->addWidget(effect_type_field_);

  connect(effect_type_field_, SIGNAL(highlighted(QString)), this, SLOT(modifyEffect(QString)));

  // Add items to the combo box
  for (auto effect_type : action_effect::EFFECT_LIST)
  {
    effect_type_field_->addItem(effect_type.c_str());
  }

  this->setLayout(effect_form_layout);
}

// ******************************************************************************************
//
// ******************************************************************************************
void EffectEditWidget::modifyEffect(const QString &text)
{
  if (umrf_)
  {
    umrf_->setEffect(text.toStdString());
  }
}

// ******************************************************************************************
//
// ******************************************************************************************
void EffectEditWidget::setUmrf(std::shared_ptr<UmrfNode> umrf)
{
  umrf_ = umrf;

  // Update the effect field
  int index = effect_type_field_->findText(umrf_->getEffect().c_str());
  if (index == -1)
  {
    // TODO: Throw an error
    return;
  }
  effect_type_field_->setCurrentIndex(index);
}

} // temoto_action_assistant namespace
