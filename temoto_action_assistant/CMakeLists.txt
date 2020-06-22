cmake_minimum_required(VERSION 2.8.12)
project(temoto_action_assistant)

add_compile_options(-std=c++11)

# definition needed for boost/math/constants/constants.hpp included by Ogre to compile
add_definitions(-DBOOST_MATH_DISABLE_FLOAT128)

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

find_package(Boost REQUIRED COMPONENTS thread filesystem system program_options)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

find_package(catkin REQUIRED COMPONENTS
  roscpp
  roslib
  std_msgs
  file_template_parser
  temoto_action_engine
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)
link_directories(${catkin_LIBRARY_DIRS})

# Qt Stuff
find_package(Qt5 REQUIRED Core Widgets Gui)
set(QT_LIBRARIES Qt5::Widgets Qt5::Core Qt5::Gui)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
add_definitions(-DQT_NO_KEYWORDS)

catkin_package(
  INCLUDE_DIRS
    include
  CATKIN_DEPENDS
    roscpp
    roslib
    std_msgs
    file_template_parser
    temoto_action_engine
)

# Header files that need Qt Moc pre-processing for use with Qt signals, etc:
set(HEADERS
  include/temoto_action_assistant/widgets/action_assistant_widget.h
  include/temoto_action_assistant/widgets/start_screen_widget.h
  include/temoto_action_assistant/widgets/setup_screen_widget.h
  include/temoto_action_assistant/widgets/header_widget.h
  include/temoto_action_assistant/widgets/navigation_widget.h
  include/temoto_action_assistant/widgets/generate_package_widget.h
  include/temoto_action_assistant/widgets/umrf_editor_widget.h
  include/temoto_action_assistant/widgets/umrf_tree_widget.h
  include/temoto_action_assistant/widgets/umrf_tree_data.h
  include/temoto_action_assistant/widgets/parameter_edit_widget.h
  include/temoto_action_assistant/widgets/parameter_group_edit_widget.h
  include/temoto_action_assistant/widgets/parameter_add_dialog_widget.h
  include/temoto_action_assistant/widgets/name_edit_widget.h
  include/temoto_action_assistant/widgets/description_edit_widget.h
  include/temoto_action_assistant/widgets/effect_edit_widget.h
  include/temoto_action_assistant/widgets/umrf_graph_widget.h
)

# Main Widgets Library - all screens (navigation options)
add_library(${PROJECT_NAME}_widgets
  src/widgets/action_assistant_widget.cpp
  src/widgets/start_screen_widget.cpp
  src/widgets/setup_screen_widget.cpp
  src/widgets/header_widget.cpp
  src/widgets/navigation_widget.cpp
  src/widgets/generate_package_widget.cpp
  src/widgets/umrf_editor_widget.cpp
  src/widgets/umrf_tree_widget.cpp
  src/widgets/parameter_edit_widget.cpp
  src/widgets/parameter_group_edit_widget.cpp
  src/widgets/parameter_add_dialog_widget.cpp
  src/widgets/name_edit_widget.cpp
  src/widgets/description_edit_widget.cpp
  src/widgets/effect_edit_widget.cpp
  src/widgets/umrf_graph_widget.cpp
  ${HEADERS}
)
set_target_properties(${PROJECT_NAME}_widgets PROPERTIES VERSION ${${PROJECT_NAME}_VERSION})
target_link_libraries(${PROJECT_NAME}_widgets
  ${QT_LIBRARIES}
  ${catkin_LIBRARIES}
  ${Boost_LIBRARIES}
)

# # # # # # # # # # # # # # # # #
#
# executables
#
# # # # # # # # # # # # # # # # #

# Action assistant GUI
add_executable(${PROJECT_NAME} 
  src/action_assistant_main.cpp
  src/ta_package_generator.cpp
  src/threaded_action_indexer.cpp
)
target_link_libraries(${PROJECT_NAME}
  ${PROJECT_NAME}_widgets 
  ${QT_LIBRARIES} 
  ${catkin_LIBRARIES} 
  ${Boost_LIBRARIES}
  log4cxx
)
