#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include "average.h"
using namespace std;
int trackNo;   //how many ENU trackfile will be merged!
int main(int argc,char** argv)
{
    int splite;
    char ENUfileName [IMLDLEN];
    double tempStamp;
    string str,subStr,temp;
    trackNo= atoi(argv[1]);
    if(trackNo==1)
    {
        mergeSimplefile(argv[2]);//if only one piecefile ,here will complete merge task
        deleteTempfile(trackNo);
        return 0;
    }
    vector<int> flag;//cloum NO. that begin to overlap 
    vector<double> startTime;//every track start timeStamp and end timeStamp
    vector<double> endTime;
    vector<int> overlap;// overlap counts of a file beginning
    vector<Point> overlapBefore,overlapBehind; //two files that overlaped have two arrays of points.
    fstream track;
    for(int i= 1; i<= trackNo; ++i)
    {
        int haveread=0;
        sprintf(ENUfileName,"%s_%d",gpsCalibrationTrack,i);
        track.open(ENUfileName,fstream::in);
        //std::cout<<"------------->"<<ENUfileName<<std::endl;
        assert(track.is_open());
        while(getline(track,str))
        {
            haveread++;
            if(haveread==1)
            {
                splite=str.find_last_of(" ");
                subStr=str.substr(0,splite);
                splite=subStr.find_last_of(" ");
                temp=subStr.substr(splite+1);
                tempStamp=atof(temp.c_str());
                startTime.push_back(tempStamp);
            }
            else
            {
                splite=str.find_last_of(" ");
                subStr=str.substr(0,splite);
                splite=subStr.find_last_of(" ");
                temp=subStr.substr(splite+1);
                tempStamp=atof(temp.c_str());
            }
        }
        endTime.push_back(tempStamp);
        track.close();
    }
    //mergefile.open("mergefile.txt",fstream::app|fstream::out);
    for(int i=1;i<=trackNo-1;++i)
    {
        int flagtemp=0;
        sprintf(ENUfileName,"%s_%d",gpsCalibrationTrack,i);
        track.open(ENUfileName,fstream::in);
        assert(track.is_open());
        while(getline(track,str))
        {   
            flagtemp++;
            splite=str.find_last_of(" ");
            subStr=str.substr(0,splite);
            splite=subStr.find_last_of(" ");
            temp=subStr.substr(splite+1);
            tempStamp=atof(temp.c_str());
            //cout<<"tempStamp= "<<tempStamp<<" startTime= "<<startTime[i]<<endl;
            if(fabs(tempStamp-startTime[i])==0)
            {
                 //cout<<"flagtemp= "<<flagtemp<<endl;
                 flag.push_back(flagtemp);
                 break;
            }
        }
        track.close();
    }
    for(int i=2;i<=trackNo;++i)
    {
        int overlaptemp=0;
        sprintf(ENUfileName,"%s_%d",gpsCalibrationTrack,i);
        track.open(ENUfileName,fstream::in);
        while(getline(track,str))
        {   overlaptemp++;
            splite=str.find_last_of(" ");
            subStr=str.substr(0,splite);
            splite=subStr.find_last_of(" ");
            temp=subStr.substr(splite+1);
            tempStamp=atof(temp.c_str());
            if(fabs(tempStamp-endTime[i-2])==0)
            {
                 overlap.push_back(overlaptemp);
                 //cerr<<"overlap "<<overlaptemp<<endl;
                 break;
            }
        }
        track.close();
    }
    if(flag.size()==overlap.size()&&flag.size()==trackNo-1)
    {
    	cout<<"flagsize=="<<flag.size()<<endl;
    	cout<<"overlapsize=="<<overlap.size()<<endl;
    }else{
    	cerr<<"piecetrack No error"<<endl;
        exit(-1);
    }
    readOverLapPointBefore(flag,overlapBefore);
    readOverLapPointBehind(overlap,overlapBehind);
    cout<<overlapBefore.size()<<"=="<<overlapBehind.size()<<endl;
    mergeTrack(flag,overlap,overlapBefore,overlapBehind,argv[2]);
    deleteTempfile(trackNo);
    return 0;
}
