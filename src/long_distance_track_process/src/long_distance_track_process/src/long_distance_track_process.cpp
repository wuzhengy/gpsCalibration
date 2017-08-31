#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "devided_segment_icp.h"
using namespace::std;
#define IMLDLEN 1024
int main(int argc,char** argv)
{
    if(argc< 7)
    {
        perror("usage: roslaunch long_distance_track_process long_distance_track_process.launch loam_track_file:=dir gps_track_file:=dir gps_enu_file:=dir loam_enu_file:=dir \
        icp_weight_file:=dir icp_type:=0/1");//0 means common icp 1 means weight icp
        return 1;
    }
    string loamTrackfile=argv[1];
    string gpsTrackfile=argv[2];
    string gpsENUfile=argv[3];
    string loamENUfile=argv[4];
    string coeICPfile=argv[5];
    int icpType=atoi(argv[6]);
    devideIcp* icp=new devideIcp();
    icp->findExcutableDir(loamTrackfile); //find excutable dir according loamtrack, this may be not necessary
	icp->makeCacheDir();//make a temporay dir to save temp file 
    icp->callGPStoLocal(loamTrackfile,gpsTrackfile,gpsENUfile);//transform WGS_84 to ENU coordinate according UTM or gaussion  projection
    icp->callRegressICP(loamENUfile,gpsENUfile,coeICPfile,icpType);//compare two similar track gps and lidar according timestamp (decision what are matched points)
    icp->deleteCacheDir();//delete the temporary directory, this probably you does not intersted in.
    delete icp;
    icp=0;
    return 0;
}
