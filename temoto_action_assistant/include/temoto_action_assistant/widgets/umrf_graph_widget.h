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
#include <QPainter>
#include <QMouseEvent>
#include <QPoint>

#ifndef Q_MOC_RUN
#endif

#include "temoto_action_engine/umrf_node.h"
#include "temoto_action_assistant/threaded_action_indexer.h"
#include <memory>
#include <map>

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

    ConnectionHelper(){}
    ConnectionHelper(const CircleHelper& other_circle, Direction direction);
    ConnectionHelper(const ConnectionHelper& ch);

    Direction direction_;
    std::string other_circle_name_;
  };

  CircleHelper(){}
  CircleHelper(const std::string& name, int x, int y, int radius);
  CircleHelper(std::shared_ptr<UmrfNode> umrf, const std::string& name, int x, int y, int radius);
  CircleHelper(const CircleHelper& ch);

  bool isInCircle(int x_in, int y_in) const;
  int isInCollisionWith(const CircleHelper& other_circle) const;
  float getRelativeDirection(const CircleHelper& other_circle) const;
  float getRelativeDistance(const CircleHelper& other_circle) const;
  QPoint getRelativeCoordinate(const float& distance, const float& angle) const;
  bool isConnectedWith(const CircleHelper& other_circle);
  bool connectWith(const CircleHelper& other_circle);
  bool disconnectWith(const CircleHelper& other_circle);
  void select();
  void unSelect();
  QPoint posAsQpoint() const;
  const std::string& getUmrfName() const;

  std::string name_;
  std::shared_ptr<UmrfNode> umrf_;
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
  UmrfGraphWidget(QWidget* parent
  , std::vector<std::shared_ptr<UmrfNode>>& umrfs
  , std::shared_ptr<ThreadedActionIndexer> action_indexer);

  void addUmrf(const UmrfNode& umrf);

  void refreshGraph();

  /// Focus given
  //void focusGiven(QTreeWidgetItem* tree_item_ptr);

  // ******************************************************************************************
  // Qt Components
  // ******************************************************************************************

Q_SIGNALS:

  // ******************************************************************************************
  // Emitted Signals
  // ******************************************************************************************
  void activeUmrfChanged(std::shared_ptr<UmrfNode> active_umrf);
  void noUmrfSelected();

private Q_SLOTS:

  // ******************************************************************************************
  // Slot Event Functions
  // ******************************************************************************************
  void addCircle();
  void addNamedCircle(QAction *action);
  void connectCircles();
  void disconnectCircles();
  void removeCircle();

private:
  // ******************************************************************************************
  // Variables
  // ******************************************************************************************

  // ******************************************************************************************
  // Private Functions
  // ******************************************************************************************
  void paintEvent(QPaintEvent* pe);
  void mousePressEvent(QMouseEvent* event);
  void mouseMoveEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void drawGraph();
  bool isInBounds(int width, int height, int x_in, int y_in);
  void setNewSelectedCircle(const std::string& new_selected_circle_);
  std::string getUniqueCircleName();
  std::vector<std::shared_ptr<UmrfNode>> getDuplicateUmrfs(const std::string& umrf_name);

  int canvas_width_, canvas_height_;
  int clicked_point_x_, clicked_point_y_;
  std::string clicked_circle_name_;
  std::string selected_circle_;
  bool circle_dropped_;
  std::map<std::string, CircleHelper> circles_;
  int circle_uniqueness_counter_;

  std::vector<std::shared_ptr<UmrfNode>>& umrfs_;
  std::shared_ptr<ThreadedActionIndexer> action_indexer_;

};
}

#endif
