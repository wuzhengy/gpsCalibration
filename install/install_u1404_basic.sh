#!/usr/bin/sh

#ROS
: << !
ROS-indigo version for ubuntu 14.04.
There are many different libraries and tools in ROS.
We provided the basic configurations to set our environment.
!
sudo sh -c 'echo "deb http://packages.ros.org/ros/ubuntu $(lsb_release -sc) main" > /etc/apt/sources.list.d/ros-latest.list';
sudo apt-key adv --keyserver hkp://ha.pool.sks-keyservers.net:80 --recv-key 421C365BD9FF1F717815A3895523BAEEB01FA116;
sudo apt-get update;
sudo apt-get install -y ros-indigo-ros-base;
sudo apt-get install -y ros-indigo-tf;
sudo apt-get install -y ros-indigo-opencv3;
sudo apt-get install -y ros-indigo-pcl-conversions;
sudo apt-get install -y ros-indigo-nav-msgs;
sudo rosdep init;
sudo rosdep update;
echo "source /opt/ros/indigo/setup.bash" >> ~/.bashrc;
source ~/.bashrc;

#PCL
: << !
The Point Cloud Library (or PCL) is a large scale,
PCL-1.7.0 is provided for ubuntu 14.04
!
sudo add-apt-repository ppa:v-launchpad-jochen-sprickerhof-de/pcl;
sudo apt-get update;
sudo apt-get install -y libpcl-all;

#EIGEN
sudo apt-get install -y libeigen3-dev;
sudo ln -s /usr/include/eigen3/Eigen /usr/include/Eigen;

#XML
sudo apt-get install -y libxml2-dev;
sudo ln -s /usr/include/libxml2/libxml /usr/include/libxml;
