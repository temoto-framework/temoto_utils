/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2012, Willow Garage, Inc.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Willow Garage nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/

#ifndef TEMOTO_ACTION_ASSISTANT_GENERATE_PACKAGE_WIDGET
#define TEMOTO_ACTION_ASSISTANT_GENERATE_PACKAGE_WIDGET

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>

#ifndef Q_MOC_RUN
#include <ros/ros.h>
#endif

#include "header_widget.h"
#include "setup_screen_widget.h"  // a base class for screens in the setup assistant
#include "temoto_action_engine/umrf.h"
#include "temoto_action_assistant/ta_package_generator.h"

namespace temoto_action_assistant
{

/**
 * \brief Start screen user interface for MoveIt Configuration Assistant
 */
class GeneratePackageWidget : public SetupScreenWidget
{
  Q_OBJECT

public:
  // ******************************************************************************************
  // Public Functions
  // ******************************************************************************************

  /**
   * \brief Start screen user interface for MoveIt Configuration Assistant
   */
  GeneratePackageWidget( QWidget* parent
  , std::string& umrf_graph_name
  , std::vector<std::shared_ptr<Umrf>>& umrfs
  , std::string temoto_actions_path
  , std::string temoto_graphs_path
  , std::string file_template_path);

  // ******************************************************************************************
  // Qt Components
  // ******************************************************************************************

  QPushButton* btn_actions_dir_;
  QPushButton* btn_graphs_dir_;
  QPushButton* btn_generate_package_;
  QLabel* next_label_;
  QProgressBar* progress_bar_;
  QLineEdit* actions_path_field_;
  QLineEdit* graphs_path_field_;

  /// Contains the data related to the action
  std::vector<std::shared_ptr<Umrf>>& umrfs_;

private Q_SLOTS:

  // ******************************************************************************************
  // Slot Event Functions
  // ******************************************************************************************
  void generatePackages();
  void setActionsPath();
  void setGraphsPath();

private:
  // ******************************************************************************************
  // Variables
  // ******************************************************************************************
  std::string temoto_actions_path_;
  std::string temoto_graphs_path_;
  std::string& umrf_graph_name_;
  ActionPackageGenerator apg_;

  // ******************************************************************************************
  // Private Functions
  // ******************************************************************************************
  std::string convertToPackageName(const std::string& name) const;
  std::string convertToClassName(const std::string& name) const;
  void generateUmrfGraph() const;

};

} // temoto action assistant namespace


#endif
