#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<vector>
#include<fstream>
#include<sstream>
#include<iomanip>

#define IMLDLEN 512
#define IMDP 15
#define SPEED (2.2)
#define DELTA 0.01

using namespace std;

struct trackCoord
{
    double x;
    double y;
    double z;
    double t;
};

int inputData(string trackFilename, vector<trackCoord> &track)
{
    char buf[IMLDLEN];
    trackCoord trackTemp;

    //clear the vector
    track.clear();

    ifstream ifile;
    ifile.open(trackFilename.c_str());
    if(!ifile)
    {
        printf("open %s error\n", trackFilename.c_str());
        return 1;
    }
   
    int IS_NULL= 1; 

    while(!ifile.eof())
    {
        bzero(buf, sizeof(buf));
        ifile.getline(buf, IMLDLEN);
        if(!strcmp(buf,"\0") && 1==IS_NULL)
        {
            return 1;
        }
        IS_NULL=0;
        
        int num=0;
        char *splite= strtok(buf," ");
        while(splite)
        {
            num++;
            if(1== num)      // col 1 is x
            {
                trackTemp.x= atof(splite);
            }
            if(2== num)      // col 2 is y
            {
                trackTemp.y= atof(splite);
            }
            if(3== num)      // col 3 is z
            {
                trackTemp.z= atof(splite);
            }
            if(4== num)      // col 4 is t
            {
                trackTemp.t= atof(splite);
                track.push_back(trackTemp);
            }
            splite=strtok(NULL," ");
        }
    }

    ifile.close();

    return 1;
}

int outputCoeData(string trackFilename, vector<float> &track)
{
    ofstream ofile;

    //open file    
    ofile.open(trackFilename.c_str());
    if(!ofile)
    {   
        printf("open %s error\n",trackFilename.c_str());
        return 1;
    }   

    //set precision
    ofile.precision(IMDP);    

    //write file
    for(int is=0; is< track.size(); is++)
    {   
        ofile<< track[is] <<endl;
    }   
    
    ofile.close();

    return 1;
}

/*
    coeSort==1:
        Speed Weight Calculation

    coeSort==2:
        Speed Weight Calculation And GPS Weight Calculation
*/

int main(int argc, char **argv)
{
    if(argc!=8)
    {
        printf("parameter error\n");
        return 1;
    }

    //get parameters from argv    
    int coeSort= atoi(argv[1]);
    string slamTrackFile= argv[2];
    string gpsTrackFile= argv[3];
    string icpTrackFile= argv[4];
    string weightCoeFile= argv[5];

    vector<trackCoord> slamTrack, icpTrack, gpsTrack, gpsPro;
    vector<float> weightSpeed, weightGPS;
    float weightTemp;
    double disx, disy, dis;
   
    if(coeSort==1)
    {
        //coe1-input slam track
        inputData(slamTrackFile, slamTrack);

        for(int is= 0; is< slamTrack.size(); is++)
        {
            if(0==is)
                weightSpeed.push_back(1.0);
            else
            {
                disx= slamTrack[is+ 1].x- slamTrack[is].x;
                disy= slamTrack[is+ 1].y- slamTrack[is].y;
                dis= sqrt(disx* disx+ disy* disy);
                weightTemp= min(dis/SPEED, 1.0);
                weightSpeed.push_back(weightTemp);
            }
        }
        //OutPut Processed Weight Coe
        outputCoeData(weightCoeFile, weightSpeed);
    }
    else
    {
        //coe2
        weightSpeed.clear();
        inputData(slamTrackFile, slamTrack);
        inputData(gpsTrackFile, gpsTrack);
        inputData(icpTrackFile, icpTrack);

        for(int is= 0; is< slamTrack.size(); is++)
        {
            if(0==is)
                weightSpeed.push_back(1.0);
            else
            {
                disx= slamTrack[is+ 1].x- slamTrack[is].x;
                disy= slamTrack[is+ 1].y- slamTrack[is].y;
                dis= sqrt(disx* disx+ disy* disy);
                weightTemp= min(dis/SPEED, 1.0);
                weightSpeed.push_back(weightTemp);
            }
        }

        double maxDis, minDis;
        maxDis= -0.1;
        minDis= 100.0;
        for(int is= 0; is< gpsTrack.size(); is++)
        {
            disx= gpsTrack[is].x- icpTrack[is].x;
            disy= gpsTrack[is].y- icpTrack[is].y;
            dis= sqrt(disx* disx+ disy* disy);
            if(dis> maxDis)
                maxDis= dis;
            if(dis< minDis)
                minDis= dis;
        }

        /*
        printf("max distance is: %lf\n", maxDis);
        printf("min distance is: %lf\n", minDis);
        printf("gps track size is: %ld\n", gpsTrack.size());
        printf("weightSpeed size is: %ld\n", weightSpeed.size());
        */

        weightGPS.clear();
        for(int is= 0; is< gpsTrack.size(); is++)
        {
            disx= gpsTrack[is].x- icpTrack[is].x;
            disy= gpsTrack[is].y- icpTrack[is].y;
            dis= sqrt(disx* disx+ disy* disy);
            weightTemp= weightSpeed[is]* 1.0/ max(DELTA, dis);
            weightGPS.push_back(weightTemp);
        }

        //OutPut Processed Weight Coe
        outputCoeData(weightCoeFile, weightGPS);
    }
    
    return 1;
}
