#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <iomanip>
#define IMSDLEN 512
const char*  PieceTrack="./data/gps_piece_track";
const char*  gpsPieceTrackWithWeight="./data/gps_piece_track_weight";
const char*  gpsCalibrationTrack= "./data/gps_calibrattion_track";
using namespace std;

struct TimeStruct
{
    double startTime;
    double endTime;
};

void getLoamTrackPieceBeginAndEndTime(char* loamTrackPieceFile,TimeStruct & startAndEnd)
{
    int row = 0;
    char buf[IMSDLEN];
    char *splite;

    // open loam track
    ifstream ifile(loamTrackPieceFile,std::ifstream::in);
    assert(ifile.is_open());
    while(ifile.getline(buf,IMSDLEN))
    {
        row++;
        if(1 == row)             // read the first row to get start time
        {
            splite = strtok(buf," ");
            int col = 0;
            while(splite)
            {
                col++;
                if(4 == col)     // the forth column is timestamp
                {
                    startAndEnd.startTime = atof(splite);
                    splite = NULL;
                    break;
                }
                splite = strtok(NULL," ");
            }
        }
        else                    
        {
            splite = strtok(buf," ");
            int col = 0;
            while(splite)
            {
                col++;
                if(4 == col)
                {
                    startAndEnd.endTime = atof(splite);        // get end time
                    splite = NULL;
                    break;
                }
                splite = strtok(NULL," ");
            }
        }
    }
    ifile.close();
}

void getGPSAndWeight(char* globalICPGPSTrack,TimeStruct & startAndEnd,int pieceNo,double &weightSum)
{
    char buf[IMSDLEN];
    char bufBack[IMSDLEN];
    char gpsPieceTrack[IMSDLEN];
    char gpsPieceTrackWeight[IMSDLEN];
    char *splite = NULL;
    string tempStr;

    sprintf(gpsPieceTrack,"%s_%d",PieceTrack,pieceNo);     // gps enu file
    sprintf(gpsPieceTrackWeight,"%s_%d",gpsPieceTrackWithWeight,pieceNo);   // weight file

    ofstream outFileXYZ(gpsPieceTrack);
    ofstream outFileWeight(gpsPieceTrackWeight);
    assert(outFileXYZ.is_open());
    assert(outFileWeight.is_open());

    ifstream ifile(globalICPGPSTrack,std::ifstream::in);
    assert(ifile.is_open());
  
    while(ifile.getline(buf,IMSDLEN))
    {
        strcpy(bufBack,buf);
        splite = strtok(buf," ");
        int col = 0;
        while(splite)
        {
            col++;
            if(4 == col)
            {
                double timeTemp = atof(splite);
                if(startAndEnd.startTime <= timeTemp && startAndEnd.endTime >= timeTemp)
                {
                    int subSize = 0;
                    string coordinateStr,weightStr;
                    tempStr = bufBack;
                    subSize = tempStr.find_last_of(" ");
                    coordinateStr = tempStr.substr(0,subSize);
                    outFileXYZ << coordinateStr << '\n';                    // save enu coordinate
                    weightStr = tempStr.substr(subSize + 1);
                    
                    weightSum += atof(weightStr.c_str());
                    outFileWeight << weightStr << '\n';                 //save weight 
                }
            }
            splite = strtok(NULL," ");
         }
    }
    ifile.close();
    outFileXYZ.close();
    outFileWeight.close();
}

//do icp
void callICP(int trackNo,char* loam_track_piece_file,int weight)
{
    char gpsPieceTrack[IMSDLEN];
    char slamPieceTrack[IMSDLEN];
    char weightTrack[IMSDLEN];
    char fileAverResedTrack[IMSDLEN];
    string command;

    sprintf(slamPieceTrack,"%s",loam_track_piece_file);
    sprintf(weightTrack,"%s_%d",gpsPieceTrackWithWeight,trackNo);
    //sprintf(fileAverResedTrack,"data/file_Aver_Resed_track_%d",trackNo);

    if(1 == weight)
    {
        sprintf(weightTrack,"%s_%d",gpsPieceTrackWithWeight,trackNo);
        sprintf(gpsPieceTrack,"%s_%d",PieceTrack,trackNo);
        sprintf(fileAverResedTrack,"%s_%d",gpsCalibrationTrack,trackNo);
        command = string("roslaunch track_icp track_icp.launch input_slamTrack_file:=") + string(slamPieceTrack) + " " + string("input_gpsTrack_file:=") + string(gpsPieceTrack) + " " + string("input_weightCoe_file:=") + string(weightTrack) + " " + string("output_reTrack_file:=") + string(fileAverResedTrack) + " " + string("icp_type:=") + std::to_string(weight);
    }
    else
    {
        sprintf(weightTrack,"%s_%d",gpsPieceTrackWithWeight,trackNo+1);
        sprintf(gpsPieceTrack,"%s_%d",gpsCalibrationTrack,trackNo);
        sprintf(fileAverResedTrack,"%s_%d",gpsCalibrationTrack,trackNo+1);
        command = string("roslaunch track_icp track_icp.launch input_slamTrack_file:=") + string(gpsPieceTrack) + " " + string("input_gpsTrack_file:=") + string(slamPieceTrack) + " " + string("input_weightCoe_file:=") + string(weightTrack) + " " + string("output_reTrack_file:=") + string(fileAverResedTrack) + " " + string("icp_type:=") + std::to_string(weight);
        //command = string("roslaunch track_icp track_icp.launch input_slamTrack_file:=") + string(slamPieceTrack) + " " + string("input_gpsTrack_file:=") + string(gpsPieceTrack) + " " + string("input_weightCoe_file:=") + string(weightTrack) + " " + string("output_reTrack_file:=") + string(fileAverResedTrack) + " " + string("icp_type:=") + std::to_string(weight);
    }
    cout<<command<<endl; 
    system(command.c_str());
    command.clear();

}
void deleteTempFile(int pieceNo)
{
    char tempfile[IMSDLEN];
    int ret=0;
    for(int i=1;i<=pieceNo;++i)
    {
        sprintf(tempfile,"%s_%d",gpsPieceTrackWithWeight,i);
        unlink(tempfile);
        sprintf(tempfile,"%s_%d",PieceTrack,i);
        ret=unlink(tempfile);
        if(ret==-1)
        {
            cerr<<"delete gps piece track file fail"<<endl;
        }
    }
}
