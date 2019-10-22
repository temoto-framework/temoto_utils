cmake_minimum_required(VERSION 2.8.3)
project(temoto_workspace_generator)
add_compile_options(-std=c++11)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  roslib
  file_template_parser
)

catkin_package(
 CATKIN_DEPENDS roscpp rospy roslib file_template_parser
)

include_directories(
# include
  ${catkin_INCLUDE_DIRS}
)

add_executable(generate_workspace 
  src/generate_workspace.cpp 
)
add_dependencies(generate_workspace
  ${catkin_EXPORTED_TARGETS}
  ${${PROJECT_NAME}_EXPORTED_TARGETS}
)
target_link_libraries(generate_workspace
  ${catkin_LIBRARIES}
)