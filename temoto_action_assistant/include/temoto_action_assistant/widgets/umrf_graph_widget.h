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

#ifndef TEMOTO_ACTION_ASSISTANT_UMRF_GRAPH_WIDGET
#define TEMOTO_ACTION_ASSISTANT_UMRF_GRAPH_WIDGET

#include <QWidget>
#include <QPixmap>
#include <QPainter>
#include <QMouseEvent>

#ifndef Q_MOC_RUN
#endif

#include "temoto_action_engine/umrf.h"

namespace temoto_action_assistant
{
class CircleHelper
{
public:
  struct ConnectionHelper
  {
    enum class Direction
    {
      INBOUND,
      OUTBOUND
    };

    ConnectionHelper(CircleHelper* other_circle, Direction direction);
    ConnectionHelper(const ConnectionHelper& ch);

    Direction direction_;
    CircleHelper* other_circle_;
  };

  CircleHelper(int x, int y, int radius);
  CircleHelper(const CircleHelper& ch);
  bool isInCircle(int x_in, int y_in) const;
  int isInCollisionWith(const CircleHelper& other_circle) const;
  float getRelativeDirection(const CircleHelper& other_circle) const;
  bool connectWith(CircleHelper& other_circle);
  void select();
  void unSelect();

  int x_, y_, radius_;
  Qt::GlobalColor border_color_;
  int border_width_;
  std::vector<ConnectionHelper> connections_;
};

class UmrfGraphWidget : public QWidget
{
  Q_OBJECT

public:
  // ******************************************************************************************
  // Public Functions
  // ******************************************************************************************

  /// Constructor
  UmrfGraphWidget(QWidget* parent = nullptr);

  /// Focus given
  //void focusGiven(QTreeWidgetItem* tree_item_ptr);

  // ******************************************************************************************
  // Qt Components
  // ******************************************************************************************

private Q_SLOTS:

  // ******************************************************************************************
  // Slot Event Functions
  // ******************************************************************************************

Q_SIGNALS:

  // ******************************************************************************************
  // Emitted Signals
  // ******************************************************************************************

private:
  // ******************************************************************************************
  // Variables
  // ******************************************************************************************
  QPixmap* pixmap_;

  // ******************************************************************************************
  // Private Functions
  // ******************************************************************************************
  void paintEvent(QPaintEvent* pe);
  void mousePressEvent(QMouseEvent* event);
  void mouseMoveEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  bool isInBounds(int width, int height, int x_in, int y_in);
  void setNewSelectedCircle(CircleHelper* new_selected_circle_);

  int canvas_width_, canvas_height_;
  CircleHelper* selected_circle_;
  bool circle_dropped_;
  std::vector<CircleHelper> circles_;
};
}

#endif
