#include <vector>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <cmath>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#define IMLDLEN 1024
#define IMDP 15
const char*  gpsCalibrationTrack= "./data/gps_calibrattion_track";
using namespace std;
struct Point{
double x;
double y;
double z;
double t;
double w;
};

extern int trackNo;
//fill overlapbefore array with points location beyound flag[i]
void readOverLapPointBefore(vector<int> flag,vector<Point>& overlapBefore)
{
    fstream track;
    char filename[IMLDLEN];
    string str;
    Point ptemp;
    for(int i=0;i<trackNo-1;++i)   
    {
        int flagtemp=0;
    	sprintf(filename,"%s_%d",gpsCalibrationTrack,i+1);
        track.open(filename,fstream::in);
        assert(track.is_open());
        while(getline(track,str))
        {   flagtemp++;
            if(flagtemp>=flag[i])
            {
                char* cstr= new char[str.length()+1];
                strcpy(cstr,str.c_str());
                char* token=strtok(cstr," ");
                int temp=0;
                while(token)
                {
                    temp++;
                    switch(temp)
                    {
                        case 1: 
                            {
                                double x=atof(token);
                                ptemp.x=x;
                                break;
                            }
                        case 2:
                            {
                                double y=atof(token);
                                ptemp.y=y;
                                break;
                            }
                        case 3:
                            {
                                double z=atof(token);
                                ptemp.z=z;
                                break;
                            }
                         case 4:
                            {
                                double t=atof(token);
                                ptemp.t=t;
                                break;
                            }
                         case 5:
                            {
                                double w=atof(token);
                                ptemp.w=w;
                                break;
                            }
                          default:
                                break;
                    }
                    token=strtok(NULL," ");
                }
                delete cstr;
                overlapBefore.push_back(ptemp);
            }
        }
        track.close();
    }
}
//fill overlapbehind array with points location within overlap[i-2]
void readOverLapPointBehind(vector<int> overlap,vector<Point>& overlapBehind)
{
    fstream track;
    char filename[IMLDLEN];
    string str;
    Point ptemp;
    for(int i=2;i<=trackNo;++i)   
    {
        int flagtemp=0;
    	sprintf(filename,"%s_%d",gpsCalibrationTrack,i);
        track.open(filename,fstream::in);
        assert(track.is_open());
        while(getline(track,str))
        {   flagtemp++;
            if(flagtemp<=overlap[i-2])
            {
                char* cstr= new char[str.length()+1];
                strcpy(cstr,str.c_str());
                char* token=strtok(cstr," ");
                int temp=0;
                while(token)
                {
                    temp++;
                    switch(temp)
                    {
                        case 1: 
                            {
                                double x=atof(token);
                                ptemp.x=x;
                                break;
                            }
                        case 2:
                            {
                                double y=atof(token);
                                ptemp.y=y;
                                break;
                            }
                        case 3:
                            {
                                double z=atof(token);
                                ptemp.z=z;
                                break;
                            }
                         case 4:
                            {
                                double t=atof(token);
                                ptemp.t=t;
                                break;
                            }
                         case 5:
                            {
                                double w=atof(token);
                                ptemp.w=w;
                                break;
                            }
                          default:
                                break;
                    }
                    token=strtok(NULL," ");
                }
                delete cstr;
                overlapBehind.push_back(ptemp);
            }
        }
        track.close();
    }
}
//calculate average and merge piece files
void calculateAverage(int flag, int startflag, int opNo, vector<Point> overlapBefore, vector<Point> overlapBehind, fstream & mergestream,int startNo)
{
    int num, smWindow;
    char buf[IMLDLEN];
    double coe1, coe2;
    double avx, avy, avz, timeStamp,avw;

    num= 1;
    coe1= 0.0;
    coe2= 0.0;
    smWindow= opNo/2;
    //cout<<"<---startNo--->"<< startNo<<" <---overlapnum--->"<<opNo<<endl;
    //cout<<"<---smWindow--->"<< smWindow<<endl;
    for(int i= startNo; i< startNo+ opNo; ++i)
    {
        //1st coe part
        if(num <= smWindow)
        {
            coe1= (1.0- num/(2.0* smWindow));
            coe2= num/(2.0* smWindow);
        }
        //2nd coe part
        else if((num> smWindow)&&(num<= opNo- smWindow))
        {
            coe1= 0.5;
            coe2= 0.5;
        }
        //3rd coe part
        else if(num> (opNo- smWindow))
        {
            coe1= (opNo- num+ 1)/(2.0* smWindow);
            coe2= (1.0- (opNo- num+ 1)/(2.0* smWindow));
        }
        avx= overlapBefore[i].x* coe1+ overlapBehind[i].x* coe2;
        avy= overlapBefore[i].y* coe1+ overlapBehind[i].y* coe2;
        avz= overlapBefore[i].z* coe1+ overlapBehind[i].z* coe2;
        timeStamp= overlapBefore[i].t;
        avw= overlapBefore[i].w* coe1+ overlapBehind[i].w* coe2;
        num++;
        mergestream<< setprecision(IMDP)<< avx<<" "<< avy<<" "<< avz<<" "<< timeStamp<<" "<<avw<<'\n';
    }

    num= 0;
    fstream temptrack;
    char filename[IMLDLEN];
    sprintf(filename,"%s_%d",gpsCalibrationTrack,flag);
    temptrack.open(filename,fstream::in);
    assert(temptrack.is_open());
    while(temptrack.getline(buf,IMLDLEN))
    {
        num++;
        if(num>opNo&& num<startflag)
        {
            mergestream<<buf<<'\n';
           //cout<<"buf=: "<<buf<<endl;
        } 
     }
     temptrack.close();
}
//get lastfile total line count
int getLastFileLine(int num)
{
    int len=0;
    char filename[IMLDLEN];
    char buf[IMLDLEN];
    sprintf(filename,"%s_%d",gpsCalibrationTrack,num);
    fstream ifile(filename,fstream::in);
    while(ifile.getline(buf,IMLDLEN)) len++;
    if(len)
    {
        cout<<"last file rows NO. "<<len<<endl;
    }
    return len;
}
//except first and last file ,other file segments have similar logical. 
int mergeTrack(vector<int> flag,vector<int> overlap,vector<Point> overlapBefore,vector<Point> overlapBehind, string mergeOverlapFile)
{
    fstream track, mergestream;
    string tempfile(gpsCalibrationTrack);
    //cout<<mergeOverlapFile<<endl;
    mergestream.open(mergeOverlapFile.c_str(),fstream::out);
    assert(mergestream.is_open());

    string firstfile=tempfile+"_1";
    track.open(firstfile.c_str(),fstream::in);
    assert(track.is_open());
    int Num=0;
    int startNo=0;
    char buf[IMLDLEN];
    while(track.getline(buf,IMLDLEN))
    {
        Num++;
        if(Num<flag[0])
        {
            mergestream<<buf<<'\n';
        }
        else {track.close();break;}
    }
    int ret=getLastFileLine(trackNo);
    for(int i=0;i<trackNo-1;++i)
    {
        if(i<trackNo-2)
        {
             calculateAverage(i+2,flag[i+1],overlap[i],overlapBefore,overlapBehind,mergestream,startNo);
             startNo+=overlap[i];
        }
        else
        calculateAverage(trackNo,ret+1,overlap[trackNo-2],overlapBefore,overlapBehind,mergestream,startNo);
    }
    mergestream<<flush;
    mergestream.close();
}
// if only one piecefile we should call this.
int mergeSimplefile(char mergeOverlapFile[])
{
    char enuTrackFile[IMLDLEN];
    char buf[IMLDLEN];
    sprintf(enuTrackFile,"%s_%d",gpsCalibrationTrack,1);
    fstream trackfile,mergefile;
    trackfile.open(enuTrackFile,fstream::in);
    mergefile.open(mergeOverlapFile,fstream::out);
    while(trackfile.getline(buf,IMLDLEN))
    {
        mergefile<<buf<<endl;
    }
    mergefile.close();
    trackfile.close();
    return 0;
}
void deleteTempfile(int trackNo)
{
    char pieceFileName[IMLDLEN];
    int ret=0;
    for(int i=1;i<=trackNo;++i)
    {
        sprintf(pieceFileName,"%s_%d",gpsCalibrationTrack,i);
        ret=unlink(pieceFileName);
        if(ret==-1)
        {
            cerr<<"delete "<< pieceFileName<<"fail"<<endl;
        }
    }

}
