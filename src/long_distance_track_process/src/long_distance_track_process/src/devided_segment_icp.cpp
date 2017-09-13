#include "devided_segment_icp.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <assert.h>
//according the file that user want to do icp find root directory of this project
string devideIcp::findExcutableDir(string loamTrackFile)
{
    size_t splite= loamTrackFile.find_last_of("/");
    string str1=loamTrackFile.substr(0,splite);
    splite=str1.find_last_of("/");
    projectDir=str1.substr(0,splite);
    cout<<projectDir<<endl;
    return projectDir;
}
//create temporary directory to save temp files
void devideIcp::makeCacheDir()
{
	cacheDir=projectDir+"/data/cacherfile/";
    cout<<"====================create temporary directory ================="<<endl;
	mkdir(cacheDir.c_str(),0777);
    sleep(2);
}
//transform WGS_84 to ENU coordinate 
int devideIcp::callGPStoLocal(string & loamTrackFile,string & gpsTrackFile ,string & gpsENUfile)
{
	//string filter_loamTrackFile=filter_dots(loamTrackFile);
	string filter_loamTrackFile=loamTrackFile;
    string gpsToLocalexectableDir=projectDir+"/devel/GPS_to_local";
    string gpsOriginalFile=gpsTrackFile;
    string projectionMethod="UTM";
    string devideMethod="3";
    string ArgumentToRun;
	string selectedGpsOriginal=cacheDir+"/gpsmatchedloam";
    //slam Matched Gps  is a file refer to temporary file matched with gps ENU coordinate timeStamp
	slamMatchedGps=cacheDir+"/slamMatchedGps";
    string gpsToLocalArgument=gpsToLocalexectableDir+" "+gpsOriginalFile+" "+projectionMethod+" "+devideMethod;
    ArgumentToRun=gpsToLocalArgument+" "+filter_loamTrackFile+" "+selectedGpsOriginal+" "+gpsENUfile+" "\
        +slamMatchedGps;
    int ret= system(ArgumentToRun.c_str());
    if(ret==-1){
            perror("run gpstolocal fail");
    }else{
            ArgumentToRun.clear();
    }
    return 0;
}
//complete robot track come from lidar and gpsENU ICP process
int devideIcp::callRegressICP(string & loamENUfile,string & gpsENUfile, string &coeFile, int icpType)
{
    string ArgumenToRun;
    ArgumenToRun="roslaunch track_icp track_icp.launch input_slamTrack_file:="+slamMatchedGps+" "+string("input_gpsTrack_file:=")+gpsENUfile+" "+\
    string("input_weightCoe_file:=")+coeFile+" "+string("output_reTrack_file:=")+loamENUfile+" "+string("icp_type:=")+std::to_string(icpType);
    cout << ArgumenToRun << endl;

    int ret=system(ArgumenToRun.c_str());
    if(ret==-1){
            perror("run regressICP fail");
            return -2;
     }else{
            ArgumenToRun.clear();
     }
     return 0;
}
void devideIcp::deleteCacheDir()
{
    DIR* pdir=opendir(cacheDir.c_str());
    assert(pdir);
    struct dirent *dir;
    struct stat st;
    while(dir=readdir(pdir))
    {
        if(strcmp(dir->d_name,".")==0||strcmp(dir->d_name,"..")==0)
        {
            continue;
        }
        string subfile=cacheDir+dir->d_name;
        if(lstat(subfile.c_str(),&st) == -1)
        {
            cerr<<"delete:lstat "<<subfile<<" error"<<endl;
            continue;
        } 
        if(S_ISREG(st.st_mode));
        {
            unlink(subfile.c_str());
        }
    }
    if(rmdir(cacheDir.c_str()) == -1)
    {
        cerr<<"delete temp error"<<endl;
    }
    closedir(pdir);
}
