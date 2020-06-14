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
#include <math.h>

namespace temoto_action_assistant
{

// ******************************************************************************************
// Constructor
// ******************************************************************************************
UmrfGraphWidget::UmrfGraphWidget(QWidget *parent)
: QWidget(parent)
, canvas_width_(400)
, canvas_height_(800)
, selected_circle_(nullptr)
{
  QVBoxLayout* v_box_layout = new QVBoxLayout(parent);
  setMinimumWidth(canvas_width_);
  setMinimumHeight(canvas_height_);

  //v_box_layout->addWidget(scroll_area);
  setLayout(v_box_layout);
}

void UmrfGraphWidget::paintEvent(QPaintEvent* pe)
{
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing,true);
  QPen pen(Qt::darkGray,2);
  painter.setPen(pen);
  painter.setBrush(Qt::lightGray);

  for (const auto& circle : circles_)
  {
    painter.drawEllipse(circle.x_ - circle.radius_
    , circle.y_ - circle.radius_
    , circle.radius_*2
    , circle.radius_*2);
  }
}

void UmrfGraphWidget::mousePressEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton)
  {
    for (auto& circle : circles_)
    {
      if (!circle.isInCircle(event->x(), event->y()))
      {
        continue;
      }
      else
      {
        selected_circle_ = &circle;
      }
    }
  }
  
  else if (event->button() == Qt::RightButton)
  {
    circles_.emplace_back(event->x(), event->y(), 25);
    update();
  }
}

void UmrfGraphWidget::mouseReleaseEvent(QMouseEvent *event)
{
  if (selected_circle_ != nullptr)
  {
    selected_circle_ = nullptr;
  }
}

void UmrfGraphWidget::mouseMoveEvent(QMouseEvent* event)
{
  if (selected_circle_ != nullptr && isInBounds(canvas_width_, canvas_height_, event->x(), event->y()))
  {
    selected_circle_->x_ = event->x();
    selected_circle_->y_ = event->y();
    update();
  }
}

bool UmrfGraphWidget::isInBounds(int width, int height, int x_in, int y_in)
{
  return x_in > 0
  && x_in < width
  && y_in > 0
  && y_in < height;
}




CircleHelper::CircleHelper(int x, int y, int radius)
: x_(x)
, y_(y)
, radius_(radius)
{}

bool CircleHelper::isInCircle(int x_in, int y_in) const
{
  int x_diff = x_ - x_in;
  int y_diff = y_ - y_in;
  int diff_length = std::sqrt(std::pow(x_diff, 2) + std::pow(y_diff, 2));

  return diff_length <= radius_;
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
