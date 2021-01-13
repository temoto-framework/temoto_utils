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

#include "temoto_action_assistant/widgets/generate_package_widget.h"
#include "temoto_action_engine/umrf_json_converter.h"
#include "std_msgs/String.h"

#include <QVBoxLayout>
#include <QFileDialog>
#include <QFormLayout>
#include <QMessageBox>

#include <boost/algorithm/string.hpp>
#include <iostream>

namespace temoto_action_assistant
{

// ******************************************************************************************
// Constructor
// ******************************************************************************************
GeneratePackageWidget::GeneratePackageWidget( QWidget* parent
, std::string& umrf_graph_name
, std::vector<std::shared_ptr<Umrf>>& umrfs
, std::string temoto_actions_path
, std::string temoto_graphs_path
, std::string file_template_path
, std::shared_ptr<ThreadedActionIndexer> action_indexer)
: SetupScreenWidget(parent),
  umrf_graph_name_(umrf_graph_name),
  umrfs_(umrfs),
  apg_(file_template_path),
  temoto_actions_path_(temoto_actions_path),
  temoto_graphs_path_(temoto_graphs_path),
  action_indexer_(action_indexer)
{
  // Layout for "add/remove selected" buttons
  QVBoxLayout* layout = new QVBoxLayout(this);

  // Add header
  HeaderWidget* header = new HeaderWidget("Generate Action Package",
                                          "Generate Action Package", this);
  layout->addWidget(header);
  layout->addSpacerItem(new QSpacerItem(1,50, QSizePolicy::Expanding, QSizePolicy::Fixed));

  /*
   * Create a form layout for the package name and path editors
   */
  QFormLayout* form_layout = new QFormLayout();

  /*
   * Add the actions path field
   */ 
  actions_path_field_ = new QLineEdit(this);
  actions_path_field_->setEnabled(false);
  actions_path_field_->insert(QString::fromStdString(temoto_actions_path_));

  // action package path selection button
  btn_actions_dir_ = new QPushButton("&Select Path", this);
  btn_actions_dir_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  btn_actions_dir_->setMaximumWidth(100);
  connect(btn_actions_dir_, SIGNAL(clicked()), this, SLOT(setActionsPath()));

  QHBoxLayout* actions_path_layout  = new QHBoxLayout();
  actions_path_layout->addWidget(actions_path_field_);
  actions_path_layout->addWidget(btn_actions_dir_);
  form_layout->addRow("Actions Path:", actions_path_layout);

  /*
   * Add the graphs path field
   */
  graphs_path_field_ = new QLineEdit(this);
  graphs_path_field_->setEnabled(false);
  graphs_path_field_->insert(QString::fromStdString(temoto_graphs_path_));

  // action package path selection button
  btn_graphs_dir_ = new QPushButton("&Select Path", this);
  btn_graphs_dir_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  btn_graphs_dir_->setMaximumWidth(100);
  connect(btn_graphs_dir_, SIGNAL(clicked()), this, SLOT(setGraphsPath()));

  QHBoxLayout* graphs_path_layout  = new QHBoxLayout();
  graphs_path_layout->addWidget(graphs_path_field_);
  graphs_path_layout->addWidget(btn_graphs_dir_);
  form_layout->addRow("Graphs Path:", graphs_path_layout);

  layout->addLayout(form_layout);
  layout->addSpacerItem(new QSpacerItem(1,50, QSizePolicy::Expanding, QSizePolicy::Fixed));

  /*
   * Generate package Button
   */
  btn_generate_package_ = new QPushButton("&Generate", this);
  btn_generate_package_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  connect(btn_generate_package_, SIGNAL(clicked()), this, SLOT(generatePackages()));
  layout->addWidget(btn_generate_package_);
  layout->setAlignment(btn_generate_package_, Qt::AlignCenter);

  if (umrfs_.size() == 0)
  {
    btn_generate_package_->setEnabled(false);
  }

  layout->addStretch();
  this->setLayout(layout);
}

// ******************************************************************************************
//
// ******************************************************************************************
void GeneratePackageWidget::setActionsPath()
{
  std::string start_dir = temoto_actions_path_;
  if (start_dir.empty())
  {
    start_dir = "~/";
  }

  QString dir = QFileDialog::getExistingDirectory(this
  , tr("Open Directory")
  , start_dir.c_str()
  , QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  if (!dir.isEmpty())
  {
    actions_path_field_->setText(dir);
    temoto_actions_path_ = dir.toStdString();
  }
}

// ******************************************************************************************
//
// ******************************************************************************************
void GeneratePackageWidget::setGraphsPath()
{
  std::string start_dir = temoto_graphs_path_;
  if (start_dir.empty())
  {
    start_dir = "~/";
  }

  QString dir = QFileDialog::getExistingDirectory(this
  , tr("Open Directory")
  , start_dir.c_str()
  , QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  if (!dir.isEmpty())
  {
    graphs_path_field_->setText(dir);
    temoto_graphs_path_ = dir.toStdString();
  }
}

// ******************************************************************************************
//
// ******************************************************************************************
void GeneratePackageWidget::generateUmrfGraph() const
{

}

// ******************************************************************************************
//
// ******************************************************************************************
void GeneratePackageWidget::generatePackages()
{
  // Make a copy of the original UMRFs
  std::vector<Umrf> umrfs_copy;
  for (const auto& original_umrf_ptr : umrfs_)
  {
    umrfs_copy.push_back(*original_umrf_ptr);
  }

  if (umrfs_.size() == 0)
  {
    QMessageBox msg_box;
    msg_box.setText("No UMRF's defined.");
    msg_box.exec();
    return;
  }

  // Check the names of the UMRFs before proceeding further
  // TODO: Make sure that there isnt any other ros packages with the same name
  for (auto& umrf_cpy : umrfs_copy)
  {
    if (umrf_cpy.getName().empty())
    {
      QMessageBox msg_box;
      msg_box.setText("One of the UMRF's has no name set.");
      msg_box.exec();
      return;
    }

    // Generate a ROS compliant package name and ROS C++ compliant class name from UMRF's name
    std::string umrf_action_package_name = convertToPackageName(umrf_cpy.getName());
    std::string umrf_action_class_name = convertToClassName(umrf_cpy.getName());

    umrf_cpy.setPackageName(umrf_action_package_name);
    umrf_cpy.setName(umrf_action_class_name);

    // Convert parent UMRF names to ROS C++ compliant format
    std::vector<Umrf::Relation> parent_relations_cpy = umrf_cpy.getParents();
    umrf_cpy.clearParents();
    for (const auto& parent_relation : parent_relations_cpy)
    {
      umrf_cpy.addParent(Umrf::Relation(convertToClassName(parent_relation.getName()), parent_relation.getSuffix()));
    }

    // Convert child UMRF names to ROS C++ compliant format
    std::vector<Umrf::Relation> child_relations_cpy = umrf_cpy.getChildren();
    umrf_cpy.clearChildren();
    for (const auto& child_relation : child_relations_cpy)
    {
      umrf_cpy.addChild(Umrf::Relation(convertToClassName(child_relation.getName()), child_relation.getSuffix()));
    }
  }

  // Generate the UMRF graph
  UmrfGraph umrf_graph(umrf_graph_name_, umrfs_copy, false);
  apg_.generateGraph(umrf_graph, temoto_graphs_path_);
  unsigned int ignored_umrfs = 0;

  // Generate TeMoto action packages
  for (auto& umrf_cpy : umrfs_copy)
  {
    if (action_indexer_->hasUmrf(umrf_cpy.getName()))
    {
      // Dont overwrite an existing package
      ignored_umrfs++;
      continue;
    }

    // Remove the "children" and "parents" fields
    umrf_cpy.clearChildren();
    umrf_cpy.clearParents();

    // Clear the examples in the parameters
    for (ActionParameters::ParameterContainer input_param : umrf_cpy.getInputParametersNc())
    {
      input_param.setExample("");
      umrf_cpy.getInputParametersNc().setParameter(input_param, true);
    }

    // Generate the TeMoto action package
    apg_.generatePackage(umrf_cpy, temoto_actions_path_);
  }

  std::string message;

  if ((umrfs_.size() - ignored_umrfs) == 1)
  {
    message = "A TeMoto Action package was generated successfully";
  }
  else if ((umrfs_.size() - ignored_umrfs) > 1)
  {
    message = std::to_string(umrfs_.size()) + " TeMoto Action packages were generated successfully";
  }

  QMessageBox msg_box;
  msg_box.setText(message.c_str());
  msg_box.exec();
}

// ******************************************************************************************
//
// ******************************************************************************************
std::string GeneratePackageWidget::convertToPackageName(const std::string& name) const
{
  /*
   * Remove whitespaces and change to lower case
   */
  std::string package_name = name;
  boost::algorithm::to_lower(package_name);
  boost::replace_all(package_name, " ", "_");

  /*
   * Remove all non alphanumeric elements except "_"
   */
  std::string path_alnum;
  for(char& c : package_name)
  {
    if (std::isalnum(c) || std::string(1, c)=="_")
    {
      path_alnum += c;
    }
  }

  /*
   * Remove repetitive "_" characters
   */
  std::string before = path_alnum;
  std::string after = path_alnum;
  do
  {
    before = after;
    boost::replace_all(after, "__", "_");
  }
  while (before != after);
  path_alnum = after;

  /*
   * Add the "ta_" prefix which stands for Temoto Action
   */
  if (!boost::contains(path_alnum, "ta_"))
  {
    path_alnum = "ta_" + path_alnum;
  }
  return path_alnum;
}

// ******************************************************************************************
//
// ******************************************************************************************
std::string GeneratePackageWidget::convertToClassName(const std::string& name) const
{
  /*
   * Create action class name
   */
  std::string package_name = convertToPackageName(name);
  std::string class_name;
  std::vector<std::string> tokens;
  boost::split(tokens, package_name, boost::is_any_of("_"));

  for (std::string token : tokens)
  {
    token[0] = std::toupper(token[0]);
    class_name += token;
  }
  return class_name;
}

} // temoto action assistant namespace
