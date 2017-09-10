#!/usr/bin/python3
from subprocess import Popen

'''
    Ubuntu 14.04 Installation Environment

        1. Robot Operating System
        
        2. Point Cloud Library

        3. EIGEN

        4. LibXML

'''

if __name__=="__main__":
        
    #ROS
    ROSSourceList= "sudo sh -c 'echo \"deb http://packages.ros.org/ros/ubuntu $(lsb_release -sc) main\" > /etc/apt/sources.list.d/ros-latest.list';"

    pROSSourceList= Popen(ROSSourceList, shell= True)
    pROSSourceList.wait()

    ROSInstallUpdate= "sudo apt-get update;"
    pROSInstallUpdate= Popen(ROSInstallUpdate, shell= True)
    pROSInstallUpdate.wait()
   
    ROSInstallBase  = "sudo apt-get install -y ros-indigo-desktop-full;" 
    pROSInstallBase= Popen(ROSInstallBase, shell= True)
    pROSInstallBase.wait()

    ROSEnvironmentSetup= "echo \"source /opt/ros/indigo/setup.bash\" >> ~/.bashrc; source ~/.bashrc;"
    pROSEnvironmentSetup= Popen(ROSEnvironmentSetup, shell= True)
    pROSEnvironmentSetup.wait()

    print("ROS has been installed!")

    #PCL
    PCLSourceList= "sudo add-apt-repository ppa:v-launchpad-jochen-sprickerhof-de/pcl; sudo apt-get update;"
    pPCLSourceList= Popen(PCLSourceList, shell=True)
    pPCLSourceList.wait()

    PCLInstall= "sudo apt-get install -y libpcl-all;"
    pPCLInstall= Popen(PCLInstall, shell=True)
    pPCLInstall.wait()

    print("PCL has been installed!")

    #EIGEN
    EigenInstall= "sudo apt-get install -y libeigen3-dev; sudo ln /usr/include/eigen3/Eigen /usr/include/Eigen;"
    pEigenInstall= Popen(EigenInstall, shell=True)
    pEigenInstall.wait()

    print("Eigen3 has been installed!")

    #XML
    XMLTool= "sudo apt-get install -y libxml2-dev; sudo ln /usr/include/libxml2/libxml /usr/include/libxml;"
    pXMLTool= Popen(XMLTool, shell=True)
    pXMLTool.wait()

    print("XML analysis lib has been installed!")

    print("Congratulations, Your evironment has been setup.")
