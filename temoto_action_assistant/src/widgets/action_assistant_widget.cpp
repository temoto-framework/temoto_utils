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

/* Author: Dave Coleman */

// SA
// #include "setup_screen_widget.h"  // a base class for screens in the setup assistant
#include "temoto_action_assistant/widgets/action_assistant_widget.h"
#include "temoto_action_engine/umrf_json_converter.h"
#include <fstream>

// Qt
#include <QListWidget>
#include <QListWidgetItem>
#include <QDebug>
#include <QFont>
#include <QLabel>
#include <QPushButton>
#include <QCloseEvent>
#include <QMessageBox>
#include <QString>

namespace temoto_action_assistant
{
// ******************************************************************************************
// Outer User Interface for MoveIt Configuration Assistant
// ******************************************************************************************
ActionAssistantWidget::ActionAssistantWidget(QWidget* parent
, boost::program_options::variables_map args)
: QWidget(parent)
, custom_parameter_map_(action_parameter::PARAMETER_MAP)
{
  // Read in the umrf
  if (args.count("du_path"))
  {
    std::string umrf_json_path = args["du_path"].as<std::string>();
    std::cout << "UMRF JSON PATH: " << umrf_json_path << std::endl;
    std::ifstream ifs(umrf_json_path);
    std::string umrf_json_str;
    umrf_json_str.assign(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
    Umrf new_umrf = umrf_json_converter::fromUmrfJsonStr(umrf_json_str, true);
    umrf_ = std::make_shared<Umrf>(new_umrf);
  }

  // Pass command arg values to start screen and show appropriate part of screen
  if (args.count("ta_path"))
  {
    temoto_actions_path_ = args["ta_path"].as<std::string>();
    std::cout << "TA PATH: " << temoto_actions_path_ << std::endl;
    // TODO: Check if the given path is valid
  }

  if (args.count("ft_path"))
  {
    file_templates_path_ = args["ft_path"].as<std::string>();
    std::cout << "FT PATH: " << file_templates_path_ << std::endl;
    // TODO: Check if the given path is valid
  }

  // Basic widget container -----------------------------------------
  QHBoxLayout* layout = new QHBoxLayout();
  layout->setAlignment(Qt::AlignTop);

  // Create main content stack for various screens
  main_content_ = new QStackedLayout();
  current_index_ = 0;

  // Wrap main_content_ with a widget
  middle_frame_ = new QWidget(this);
  middle_frame_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  middle_frame_->setLayout(main_content_);

  // Screens --------------------------------------------------------

  // Start Screen
  ssw_ = new StartScreenWidget(this, umrf_, file_templates_path_);
  ssw_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  connect(ssw_, SIGNAL(readyToProgress()), this, SLOT(progressPastStartScreen()));
  main_content_->addWidget(ssw_);

  if (args.count("config_pkg"))
  {
    ssw_->stack_path_->setPath(args["config_pkg"].as<std::string>());
    ssw_->select_mode_->btn_exist_->click();
  }
  else
  {
    // Open the directory where the MSA was started from.
    // cf. http://stackoverflow.com/a/7413516/577001
    QString pwdir("");
    char* pwd;
    pwd = getenv("PWD");
    std::cout << "TODO REMOVE ... here: " << pwd << std::endl;
    pwdir.append(pwd);
    ssw_->stack_path_->setPath(pwdir);
  }

  // Packgage generation screen

  // Add Navigation Buttons (but do not load widgets yet except start screen)
  nav_name_list_ << "Start";
  nav_name_list_ << "UMRF Editor";
  nav_name_list_ << "Generate Action";

  // // Navigation Left Pane --------------------------------------------------
  navs_view_ = new NavigationWidget(this);
  navs_view_->setNavs(nav_name_list_);
  navs_view_->setDisabled(true);
  navs_view_->setSelected(0);  // start screen

  // Split screen -----------------------------------------------------
  splitter_ = new QSplitter(Qt::Horizontal, this);
  splitter_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  splitter_->addWidget(navs_view_);
  splitter_->addWidget(middle_frame_);
  splitter_->setHandleWidth(6);
  // splitter_->setCollapsible( 0, false ); // don't let navigation collapse
  layout->addWidget(splitter_);
  // Add event for switching between screens -------------------------
  connect(navs_view_, SIGNAL(clicked(const QModelIndex&)), this, SLOT(navigationClicked(const QModelIndex&)));

  // Final Layout Setup ---------------------------------------------
  this->setLayout(layout);

  // Title
  this->setWindowTitle("TeMoto Action Assistant");  // title of window

  // Show screen before message
  QApplication::processEvents();
}

// ******************************************************************************************
// Decontructor
// ******************************************************************************************
ActionAssistantWidget::~ActionAssistantWidget(){}

// ******************************************************************************************
// Change screens of Setup Assistant
// ******************************************************************************************
void ActionAssistantWidget::navigationClicked(const QModelIndex& index)
{
  // Convert QModelIndex to int
  moveToScreen(index.row());
}

// ******************************************************************************************
// Change screens
// ******************************************************************************************
void ActionAssistantWidget::moveToScreen(const int index)
{
  boost::mutex::scoped_lock slock(change_screen_lock_);

  if (current_index_ != index)
  {
    // Send the focus lost command to the screen widget
    SetupScreenWidget* ssw = qobject_cast<SetupScreenWidget*>(main_content_->widget(current_index_));
    if (!ssw->focusLost())
    {
      navs_view_->setSelected(current_index_);
      return;  // switching not accepted
    }

    current_index_ = index;

    // Change screens
    main_content_->setCurrentIndex(index);

    // Send the focus given command to the screen widget
    ssw = qobject_cast<SetupScreenWidget*>(main_content_->widget(index));
    ssw->focusGiven();

    // Change navigation selected option
    navs_view_->setSelected(index);  // Select first item in list
  }
}

// ******************************************************************************************
// Loads other windows, enables navigation
// ******************************************************************************************
void ActionAssistantWidget::progressPastStartScreen()
{
  // Load all widgets ------------------------------------------------

  // UMRF Editor
  uew_ = new UmrfEditorWidget(this, umrf_, &custom_parameter_map_);
  main_content_->addWidget(uew_);
  connect(uew_, SIGNAL(isModal(bool)), this, SLOT(setModalMode(bool)));

  // Package generator widget
  gpw_ = new GeneratePackageWidget(this, umrf_, temoto_actions_path_, file_templates_path_);
  main_content_->addWidget(gpw_);

  // Enable all nav buttons -------------------------------------------
  for (int i = 0; i < nav_name_list_.count(); ++i)
  {
    navs_view_->setEnabled(i, true);
  }

  // Enable navigation
  navs_view_->setDisabled(false);
}

// ******************************************************************************************
// Ping ROS on internval
// ******************************************************************************************
void ActionAssistantWidget::updateTimer()
{
  ros::spinOnce();  // keep ROS node alive
}

// ******************************************************************************************
// Qt close event function for reminding user to save
// ******************************************************************************************
void ActionAssistantWidget::closeEvent(QCloseEvent* event)
{
  if (QMessageBox::question(this, "Exit Setup Assistant",
                            QString("Are you sure you want to exit the TeMoto Action Assistant?"),
                            QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Cancel)
  {
    event->ignore();
    return;
  }

  // Shutdown app
  event->accept();
}

// ******************************************************************************************
// Qt Error Handling - TODO
// ******************************************************************************************
bool ActionAssistantWidget::notify(QObject* reciever, QEvent* event)
{
  QMessageBox::critical(this, "Error", "An error occurred and was caught by Qt notify event handler.", QMessageBox::Ok);

  return false;
}

// ******************************************************************************************
// Change the widget modal state based on subwidgets state
// ******************************************************************************************
void ActionAssistantWidget::setModalMode(bool isModal)
{
  navs_view_->setDisabled(isModal);

  for (int i = 0; i < nav_name_list_.count(); ++i)
  {
    navs_view_->setEnabled(i, !isModal);
  }
}

}  // namespace
