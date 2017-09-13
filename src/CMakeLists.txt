cmake_minimum_required(VERSION 2.8.3)
project(iMorpheusAI)

## Find catkin macros and libraries
## is used, also find other catkin packages
##find_packages that we will compile 

add_subdirectory(lidar_slam)
add_subdirectory(gps_process)
add_subdirectory(long_distance_track_process)
add_subdirectory(icp_weight_coe)
add_subdirectory(short_distance_track_process)
add_subdirectory(track_icp)


