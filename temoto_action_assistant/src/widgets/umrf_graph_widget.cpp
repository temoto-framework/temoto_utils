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
#include <QMenu>
#include <iostream>
#include <math.h>

namespace temoto_action_assistant
{

// ******************************************************************************************
// Constructor
// ******************************************************************************************
UmrfGraphWidget::UmrfGraphWidget(QWidget *parent, std::vector<std::shared_ptr<Umrf>>& umrfs)
: QWidget(parent)
, umrfs_(umrfs)
, canvas_width_(400)
, canvas_height_(800)
, selected_circle_("")
, circle_dropped_(true)
, circle_uniqueness_counter_(0)
{
  QVBoxLayout* v_box_layout = new QVBoxLayout(parent);
  v_box_layout->setContentsMargins(0, 0, 0, 0);
  
  setMinimumWidth(canvas_width_);
  setMinimumHeight(canvas_height_);
  setLayout(v_box_layout);

  for (auto& umrf : umrfs_)
  {
    // TODO: Pass as lvalue reference
    std::string unique_circle_name = getUniqueCircleName();
    circles_.insert({unique_circle_name, CircleHelper(umrf, unique_circle_name, 170, 80, 25)});
  }
}

void UmrfGraphWidget::paintEvent(QPaintEvent* pe)
{
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing,true);
  painter.setBrush(Qt::darkGray);

  // First draw the lines, so that the lines will always be below circles
  for (const auto& circle : circles_)
  {
    QPen line_pen(Qt::darkGray, 2);
    QPen arrow_pen;
    arrow_pen.setStyle(Qt::PenStyle::NoPen);

    // Draw the connection lines
    for (const auto& connection : circle.second.connections_)
    {
      if (connection.direction_ != CircleHelper::ConnectionHelper::Direction::OUTBOUND)
      {
        continue;
      }
      if (circles_.find(connection.other_circle_name_) == circles_.end())
      {
        std::cout << "Cannot find cirle '" << connection.other_circle_name_ << "'" << std::endl;
        continue;
      }
      
      painter.setPen(line_pen);
      painter.drawLine(circle.second.x_
      , circle.second.y_
      , circles_[connection.other_circle_name_].x_
      , circles_[connection.other_circle_name_].y_);

      // Build and draw the arrow head
      float direction = circle.second.getRelativeDirection(circles_[connection.other_circle_name_]);
      float distance = circle.second.getRelativeDistance(circles_[connection.other_circle_name_]);
      float arrow_length = 14;
      float other_circle_radius = circles_[connection.other_circle_name_].radius_;
      float d_direction = 6/(distance - other_circle_radius - arrow_length);

      QPoint arrow_vertex_0 = circle.second.getRelativeCoordinate(distance - other_circle_radius - arrow_length, direction + d_direction);
      QPoint arrow_vertex_1 = circle.second.getRelativeCoordinate(distance - other_circle_radius - arrow_length, direction - d_direction);
      QPoint arrow_vertex_2 = circle.second.getRelativeCoordinate(distance - other_circle_radius, direction);

      QPolygon arrow_head;
      arrow_head.clear();
      arrow_head << arrow_vertex_0 << arrow_vertex_1 << arrow_vertex_2;
      painter.setPen(arrow_pen);
      painter.drawPolygon(arrow_head);
    }
  }

  // Draw circle shadows
  QPen shadow_pen;
  shadow_pen.setStyle(Qt::PenStyle::NoPen);
  painter.setPen(shadow_pen);
  painter.setBrush(Qt::darkGray);

  for (const auto& circle : circles_)
  {
    painter.drawEllipse(circle.second.x_ - circle.second.radius_ + 2
    , circle.second.y_ - circle.second.radius_ + 2
    , circle.second.radius_*2
    , circle.second.radius_*2);
  }

  // Draw the circles
  painter.setBrush(Qt::lightGray);
  QPen text_pen(Qt::black, 1);
  for (const auto& circle : circles_)
  {
    QPen body_pen;
    if (circle.second.name_ == selected_circle_)
    {
      body_pen = QPen(circle.second.border_color_, circle.second.border_width_);
    }
    else
    {
      body_pen.setStyle(Qt::PenStyle::NoPen);
    }
    painter.setPen(body_pen);
    
    painter.drawEllipse(circle.second.x_ - circle.second.radius_
    , circle.second.y_ - circle.second.radius_
    , circle.second.radius_*2
    , circle.second.radius_*2);

    painter.setPen(text_pen);
    painter.drawText(circle.second.x_ + 25, circle.second.y_ - 14, QString(circle.second.getUmrfName().c_str()));
  }
}

void UmrfGraphWidget::mousePressEvent(QMouseEvent* event)
{
  QMenu menu(this);
  clicked_point_x_ = event->x();
  clicked_point_y_ = event->y();
  clicked_circle_name_ = "";

  // Check if any circles were clicked
  for (auto& circle : circles_)
  {
    if (circle.second.isInCircle(event->x(), event->y()))
    {
      clicked_circle_name_ = circle.first;
      break;  
    }
  }

  // If a circle was clicked with left button then select the circle
  if (!clicked_circle_name_.empty() && event->button() == Qt::LeftButton)
  {
    setNewSelectedCircle(clicked_circle_name_);
    circle_dropped_ = false;
    update();
    return;
  }
  // If empty space was left-clicked, then unselect the selected cirlcle
  else if (clicked_circle_name_.empty() && event->button() == Qt::LeftButton)
  {
    if (!selected_circle_.empty())
    {
      circles_[selected_circle_].unSelect();
      selected_circle_ = "";
    }
    update();
    Q_EMIT noUmrfSelected();
    return;
  }
  // If empty space was right-clicked, then create a new circle
  else if (clicked_circle_name_.empty() && event->button() == Qt::RightButton)
  {
    QAction* add_action = new QAction(tr("&ADD Action"), this);
    connect(add_action, SIGNAL(triggered()), this, SLOT(addCircle()));
    menu.addAction(add_action);
  }
  // If a circle was clicked with right button then connect/disconnect the selected cirle to it
  else if (!clicked_circle_name_.empty() && event->button() == Qt::RightButton)
  {
    // TODO: the order of evaluoation obviously matters, but its not a reliable code design
    if (!selected_circle_.empty() && circles_[selected_circle_].isConnectedWith(circles_[clicked_circle_name_]))
    {
      QAction* disconnect_action = new QAction(tr("&Disconnect"), this);
      connect(disconnect_action, SIGNAL(triggered()), this, SLOT(disconnectCircles()));
      menu.addAction(disconnect_action);
    }
    else if (selected_circle_ != clicked_circle_name_ && !selected_circle_.empty())
    {
      QAction* connect_action = new QAction(tr("&Connect"), this);
      connect(connect_action, SIGNAL(triggered()), this, SLOT(connectCircles()));
      menu.addAction(connect_action);
    }
    else if((selected_circle_ == clicked_circle_name_) || selected_circle_.empty())
    {
      QAction* remove_action = new QAction(tr("&Remove"), this);
      connect(remove_action, SIGNAL(triggered()), this, SLOT(removeCircle()));
      menu.addAction(remove_action);
    }
  }
  
  menu.exec(this->mapToGlobal(event->pos()));
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
      if (first_circle.first == second_circle.first)
      {
        continue;
      }

      int overlap = first_circle.second.isInCollisionWith(second_circle.second);

      // Ignore non-colliding circles
      if (overlap == 0)
      {
        continue;
      }

      float direction = first_circle.second.getRelativeDirection(second_circle.second);
      int x_overlap = (overlap + 10) * std::cos(direction);
      int y_overlap = (overlap + 10) * std::sin(direction);
      second_circle.second.x_ -= x_overlap;
      second_circle.second.y_ -= y_overlap; 
    }
  }
  update();
}

void UmrfGraphWidget::mouseMoveEvent(QMouseEvent* event)
{
  if (!circle_dropped_ 
  && isInBounds(canvas_width_, canvas_height_, event->x(), event->y())
  && !selected_circle_.empty())
  {
    circles_[selected_circle_].x_ = event->x();
    circles_[selected_circle_].y_ = event->y();
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

void UmrfGraphWidget::setNewSelectedCircle(const std::string& new_selected_circle_)
{
  if (!selected_circle_.empty())
  {
    circles_[selected_circle_].unSelect();
  }

  circles_[new_selected_circle_].select();
  selected_circle_ = new_selected_circle_;
  Q_EMIT activeUmrfChanged(circles_[new_selected_circle_].umrf_);
}

void UmrfGraphWidget::addCircle()
{
  std::string unique_circle_name = getUniqueCircleName();
  circles_.insert({unique_circle_name, CircleHelper(unique_circle_name, clicked_point_x_, clicked_point_y_, 25)});
  umrfs_.push_back(circles_[unique_circle_name].umrf_);

  setNewSelectedCircle(unique_circle_name);
  update();
}

void UmrfGraphWidget::connectCircles()
{
  circles_[selected_circle_].connectWith(circles_[clicked_circle_name_]);
  circles_[selected_circle_].umrf_->addChild(circles_[clicked_circle_name_].umrf_->getName());
  circles_[clicked_circle_name_].umrf_->addParent(circles_[selected_circle_].umrf_->getName());
  update();
}

void UmrfGraphWidget::disconnectCircles()
{
  circles_[selected_circle_].disconnectWith(circles_[clicked_circle_name_]);
  circles_[selected_circle_].umrf_->removeChild(circles_[clicked_circle_name_].umrf_->getName());
  circles_[clicked_circle_name_].umrf_->removeParent(circles_[selected_circle_].umrf_->getName());
  update();
}

void UmrfGraphWidget::removeCircle()
{
  std::string erased_umrf_name = circles_[clicked_circle_name_].umrf_->getName();

  // Erase the UMRF from umrfs_ vector
  // TODO: At the moment the match between the umrf in umrfs_ vector and
  // the umrf in the circle class is found by comparing the addresses of
  // the pointed objects. This is for sure not pretty and could be done
  // in a more safer way. 
  auto umrf_iterator = std::find_if(umrfs_.begin(), umrfs_.end()
  , [&](const std::shared_ptr<Umrf>& u) 
  {
    if (&(*u) == &(*circles_[clicked_circle_name_].umrf_))
      return true;
    else
      return false;
  });

  // Erase the UMRF from the globally shared vector of UMRF pointers
  if (umrf_iterator != umrfs_.end())
  {
    umrfs_.erase(umrf_iterator);
  }
  else
  {
    std::cout << "Cannot remove the circle because the UMRF which was contained"
    "within the graph GUI was not found" << std::endl;
    return;
  }

  // Erase the circle
  circles_.erase(clicked_circle_name_);
  if (clicked_circle_name_ == selected_circle_)
  {
    selected_circle_ = "";
  }

  // Remove all inbound connections
  for (auto& circle : circles_)
  {
    // Remove the erased umrf from other umrfs. If the erased umrf was not
    // child/parent, then nothing happens.W
    circle.second.umrf_->removeParent(erased_umrf_name);
    circle.second.umrf_->removeChild(erased_umrf_name);

    for (auto con_it=circle.second.connections_.begin()
    ; con_it!=circle.second.connections_.end()
    ; con_it++)
    {
      if (con_it->other_circle_name_ == clicked_circle_name_)
      {
        circle.second.connections_.erase(con_it);
        break;
      }
      else
      {
        continue;
      }
    }
  }
  update();
}

void UmrfGraphWidget::addUmrf(const Umrf& umrf)
{
  const std::string unique_circle_name = getUniqueCircleName();

  // Get the y coordinate of the lowest circle
  int max_y_pos = 30;
  for (const auto& circle : circles_)
  {
    if (circle.second.y_ > max_y_pos)
    {
      max_y_pos = circle.second.y_ + 85;
    }
  }

  Umrf local_umrf = umrf;
  local_umrf.setName(local_umrf.getDescription());

  for (const auto& circle : circles_)
  {
    if (circle.second.umrf_->getName() == local_umrf.getName())
    {
      std::cout << "The provided UMRF already exists" << std::endl;
      return;
    }
  }

  circles_.insert({unique_circle_name, CircleHelper(std::make_shared<Umrf>(local_umrf), unique_circle_name, 100, max_y_pos, 25)});
  umrfs_.push_back(circles_[unique_circle_name].umrf_);
 
  setNewSelectedCircle(unique_circle_name);
  update();
}

std::string UmrfGraphWidget::getUniqueCircleName()
{
  return "action_" + std::to_string(circle_uniqueness_counter_++);
}


// ******************************************************************************************
// Circle and Line helper definitions
// ******************************************************************************************

CircleHelper::CircleHelper(const std::string& name, int x, int y, int radius)
: name_(name)
, x_(x)
, y_(y)
, radius_(radius)
, border_color_(Qt::darkGray)
, border_width_(2)
{
  Umrf umrf;
  umrf.setName(name);
  umrf.setEffect("synchronous");
  umrf_ = std::make_shared<Umrf>(umrf);
}

CircleHelper::CircleHelper(std::shared_ptr<Umrf> umrf, const std::string& name, int x, int y, int radius)
: umrf_(umrf)
, name_(name)
, x_(x)
, y_(y)
, radius_(radius)
, border_color_(Qt::darkGray)
, border_width_(2)
{}

CircleHelper::CircleHelper(const CircleHelper& ch)
: umrf_(ch.umrf_)
, name_(ch.name_)
, x_(ch.x_)
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
  int diff_length = getRelativeDistance(other_circle);
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

float CircleHelper::getRelativeDistance(const CircleHelper& other_circle) const
{
  int x_diff = x_ - other_circle.x_;
  int y_diff = y_ - other_circle.y_;

  return std::sqrt(std::pow(x_diff, 2) + std::pow(y_diff, 2));
}

QPoint CircleHelper::getRelativeCoordinate(const float& distance, const float& angle) const
{
  QPoint coordinate;
  coordinate.setX(x_ - distance * std::cos(angle));
  coordinate.setY(y_ - distance * std::sin(angle));

  return coordinate;
}

bool CircleHelper::isConnectedWith(const CircleHelper& other_circle)
{
  for (const auto& connection : connections_)
  {
    if (connection.other_circle_name_ == other_circle.name_)
    {
      return true;
    }
  }
  return false;
}

bool CircleHelper::connectWith(const CircleHelper& other_circle)
{
  // Do not connect to itself
  if (name_ == other_circle.name_)
  {
    return false;
  }

  // Do not connect to existing connection
  for (const auto& connection : connections_)
  {
    if (connection.other_circle_name_ == other_circle.name_)
    {
      return false;
    }
  }

  connections_.emplace_back(other_circle, ConnectionHelper::Direction::OUTBOUND);
  return true;
}

bool CircleHelper::disconnectWith(const CircleHelper& other_circle)
{
  // Do not connect to itself
  if (name_ == other_circle.name_)
  {
    return false;
  }

  for (auto it=connections_.begin(); it!=connections_.end(); it++)
  {
    if (it->other_circle_name_ != other_circle.name_)
    {
      continue;
    }
    else
    {
      connections_.erase(it);
      return true;
    }
  }
  return false;
}

void CircleHelper::select()
{
  border_width_ = 2;
  border_color_ = Qt::black;
}

void CircleHelper::unSelect()
{
  border_width_ = 2;
  border_color_ = Qt::darkGray;
}

const std::string& CircleHelper::getUmrfName() const
{
  return umrf_->getName();
}

QPoint CircleHelper::posAsQpoint() const
{
  return QPoint(x_, y_);
}

CircleHelper::ConnectionHelper::ConnectionHelper(const CircleHelper& other_circle, Direction direction)
: other_circle_name_(other_circle.name_)
, direction_(direction)
{}

CircleHelper::ConnectionHelper::ConnectionHelper(const ConnectionHelper& ch)
: other_circle_name_(ch.other_circle_name_)
, direction_(ch.direction_)
{}

} // temoto_action_assistant namespace
