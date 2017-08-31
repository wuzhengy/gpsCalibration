cmake_minimum_required(VERSION 2.8.3)
project(track_icp)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
)

## System dependencies are found with CMake's conventions
# find_package(Boost REQUIRED COMPONENTS system)

catkin_package(
  #INCLUDE_DIRS include
  LIBRARIES track_icp
  CATKIN_DEPENDS roscpp std_msgs
  DEPENDS system_lib
)


## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
 include
  ${catkin_INCLUDE_DIRS}
)

add_executable(track_icp
  src/track_icp.cpp
)
add_dependencies(track_icp ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(track_icp
  ${catkin_LIBRARIES}
)
