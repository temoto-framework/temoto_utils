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

#include "temoto_action_assistant/widgets/umrf_graph_widget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <iostream>

namespace temoto_action_assistant
{

// ******************************************************************************************
// Constructor
// ******************************************************************************************
UmrfGraphWidget::UmrfGraphWidget(QWidget *parent)
: QWidget(parent)
{
  QVBoxLayout* v_box_layout = new QVBoxLayout;


  //v_box_layout->addWidget(scroll_area);
  setLayout(v_box_layout);
}

void UmrfGraphWidget::paintEvent(QPaintEvent* pe)
{
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing,true);
  QPen pen(Qt::blue,2);
  painter.setPen(pen);
  QRect r=QRect(0,0,100,100);
  painter.drawEllipse(r);
}

// ******************************************************************************************
//
// ******************************************************************************************
// void UmrfGraphWidget::focusGiven(QTreeWidgetItem* tree_item_ptr)
// {
//   // Set the tree item pointer to active element
//   tree_item_ptr_ = tree_item_ptr;
//   tree_data_ = tree_item_ptr_->data(0, Qt::UserRole).value<UmrfTreeData>();
//   std::string* name = boost::any_cast<std::string*>(tree_data_.payload_);

//   /*
//    * Update the word field
//    */
//   name_field_->setText(QString::fromStdString(*name));
// }


} // temoto_action_assistant namespace
