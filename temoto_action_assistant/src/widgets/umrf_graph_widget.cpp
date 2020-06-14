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
, circle_dropped_(true)
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
  painter.setBrush(Qt::lightGray);

  // First draw the lines, so that the lines will always be below circles
  for (const auto& circle : circles_)
  {
    if (&circle == selected_circle_)
    {
      QPen pen(Qt::black, 3);
      painter.setPen(pen);
    }
    else
    {
      QPen pen(circle.border_color_, circle.border_width_);
      painter.setPen(pen);
    }

    // Draw the connection lines
    for (const auto& connection : circle.connections_)
    {
      if (connection.direction_ != CircleHelper::ConnectionHelper::Direction::OUTBOUND)
      {
        continue;
      }
      painter.drawLine(circle.x_, circle.y_, connection.other_circle_->x_, connection.other_circle_->y_);
    }
  }

  // Draw the circles
  for (const auto& circle : circles_)
  {
    if (&circle == selected_circle_)
    {
      QPen pen(Qt::black, 3);
      painter.setPen(pen);
    }
    else
    {
      QPen pen(circle.border_color_, circle.border_width_);
      painter.setPen(pen);
    }

    painter.drawEllipse(circle.x_ - circle.radius_
    , circle.y_ - circle.radius_
    , circle.radius_*2
    , circle.radius_*2);
  }
}

void UmrfGraphWidget::mousePressEvent(QMouseEvent* event)
{
  bool circle_clicked = false;
  for (auto& circle : circles_)
  {
    if (!circle.isInCircle(event->x(), event->y()))
    {
      continue;  
    }
    
    if (event->button() == Qt::LeftButton)
    {
      setNewSelectedCircle(&circle);
      circle_dropped_ = false;
      return;
    }
    else if (event->button() == Qt::RightButton)
    {
      if (selected_circle_ != nullptr)
      {
        selected_circle_->connectWith(circle);
      }
      return;
    }
    else
    {
      break;
    }
  }

  // If empty space was right-clicked, then create a new circle
  if (!circle_clicked && event->button() == Qt::RightButton)
  {
    circles_.emplace_back(event->x(), event->y(), 25);
    setNewSelectedCircle(&(circles_.back()));
  }
  // If empty space was left-clicked, then unselect the selected cirlcle
  else
  {
    if (selected_circle_ != nullptr)
    {
      selected_circle_->unSelect();
      selected_circle_ = nullptr;
    }
  }
  
  update();
}

void UmrfGraphWidget::mouseReleaseEvent(QMouseEvent *event)
{
  circle_dropped_ = true;

  // Push the circles that are in collision apart from each oter
  for (const auto& first_circle : circles_)
  {
    for (auto& second_circle : circles_)
    {
      // Ignore itself
      if (&first_circle == &second_circle)
      {
        continue;
      }

      int overlap = first_circle.isInCollisionWith(second_circle);

      // Ignore non-colliding circles
      if (overlap == 0)
      {
        continue;
      }

      float direction = first_circle.getRelativeDirection(second_circle);
      int x_overlap = (overlap + 10) * std::cos(direction);
      int y_overlap = (overlap + 10) * std::sin(direction);
      second_circle.x_ -= x_overlap;
      second_circle.y_ -= y_overlap; 
    }
  }
  update();
}

void UmrfGraphWidget::mouseMoveEvent(QMouseEvent* event)
{
  if (!circle_dropped_ && isInBounds(canvas_width_, canvas_height_, event->x(), event->y()))
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

void UmrfGraphWidget::setNewSelectedCircle(CircleHelper* new_selected_circle_)
{
  // TODO: this is the most rediculous bug i've ever encountered. If the following
  // section of code is uncommented, then every 1, 2, 4, 8, 16, 32, ... 2^n 
  // circle will stay in "selected" state

  // if (selected_circle_ != nullptr)
  // {
  //   selected_circle_->unSelect();
  // }

  // new_selected_circle_->select();
  selected_circle_ = new_selected_circle_;
}

// ******************************************************************************************
// Circle and Line helper definitions
// ******************************************************************************************

CircleHelper::CircleHelper(int x, int y, int radius)
: x_(x)
, y_(y)
, radius_(radius)
, border_color_(Qt::darkGray)
, border_width_(2)
{}

CircleHelper::CircleHelper(const CircleHelper& ch)
: x_(ch.x_)
, y_(ch.y_)
, radius_(ch.radius_)
, border_color_(ch.border_color_)
, border_width_(ch.border_width_)
{
  for (const auto& connection : ch.connections_)
  {
    connections_.push_back(connection);
  }
}

bool CircleHelper::isInCircle(int x_in, int y_in) const
{
  int x_diff = x_ - x_in;
  int y_diff = y_ - y_in;
  int diff_length = std::sqrt(std::pow(x_diff, 2) + std::pow(y_diff, 2));

  return diff_length <= radius_;
}

int CircleHelper::isInCollisionWith(const CircleHelper& other_circle) const
{
  int x_diff = x_ - other_circle.x_;
  int y_diff = y_ - other_circle.y_;
  int diff_length = std::sqrt(std::pow(x_diff, 2) + std::pow(y_diff, 2));
  int overlap = 2*radius_ - diff_length;
  
  if (overlap <= 0)
  {
    return 0;
  }
  else
  {
    return overlap;
  }
}

float CircleHelper::getRelativeDirection(const CircleHelper& other_circle) const
{
  int x_diff = x_ - other_circle.x_;
  int y_diff = y_ - other_circle.y_;

  return std::atan2(y_diff, x_diff);
}

bool CircleHelper::connectWith(CircleHelper& other_circle)
{
  // Do not connect to itself
  if (this == &other_circle)
  {
    return false;
  }

  // Do not connect to existing connection
  for (const auto& connection : connections_)
  {
    if (connection.other_circle_ == &other_circle)
    {
      return false;
    }
  }

  connections_.emplace_back(&other_circle, ConnectionHelper::Direction::OUTBOUND);
  return true;
}

void CircleHelper::select()
{
  border_width_ = 3;
  border_color_ = Qt::black;
}

void CircleHelper::unSelect()
{
  border_width_ = 2;
  border_color_ = Qt::darkGray;
}

CircleHelper::ConnectionHelper::ConnectionHelper(CircleHelper* other_circle, Direction direction)
: other_circle_(other_circle)
, direction_(direction)
{}

CircleHelper::ConnectionHelper::ConnectionHelper(const ConnectionHelper& ch)
: other_circle_(ch.other_circle_)
, direction_(ch.direction_)
{}

} // temoto_action_assistant namespace
