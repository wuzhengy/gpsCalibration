#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


void interPolate(vector<pair<double,double> >&LocalXY,vector<double> &GPSTime,vector<double> &TrailTime,vector<pair<double,double> > &IterLocalXY)
{

    double *dataX=(double *)malloc(sizeof(double)*LocalXY.size());
    double *dataY=(double *)malloc(sizeof(double)*LocalXY.size());

    double *timeInputSource=(double *)malloc(sizeof(double)*GPSTime.size());
    double *timeInputRefer=(double *)malloc(sizeof(double)*TrailTime.size());

    double *dataInterX=(double *)malloc(sizeof(double)*TrailTime.size());
    double *dataInterY=(double *)malloc(sizeof(double)*TrailTime.size());

    vector<pair<double,double> >::iterator iterLocalXY;
    int index=0;
    for(iterLocalXY=LocalXY.begin();iterLocalXY!=LocalXY.end();iterLocalXY++)
    {
	dataX[index]=iterLocalXY->first;
	dataY[index]=iterLocalXY->second;
	++index;
    }    

    index=0;
    vector<double>::iterator iterGPS;
    for(iterGPS=GPSTime.begin();iterGPS!=GPSTime.end();iterGPS++)
    {
        timeInputSource[index++]=(*iterGPS);
    } 
    int totalNumSource=GPSTime.size();
    cout<<"totalNumSource="<<GPSTime.size()<<endl;

    index=0;
    vector<double>::iterator iterTrail;
    for(iterTrail=TrailTime.begin();iterTrail!=TrailTime.end();iterTrail++)
    {
        timeInputRefer[index++]=(*iterTrail);
    }
    int totalNumRefer=TrailTime.size();
    cout<<"totoalNumRefer="<<TrailTime.size()<<endl;
    

    //Input GPS track coordinates
    /* Do the interpolation
        1. interpolate gps track at each reference time
        2. interpolation method- linear interpolation
    */
    double s1, s2, s3;
    float x1, x2, y1, y2, coe1, coe2;
    int iCount= 0;

   
    int iSource,iRefer;
    for(iSource= 0; iSource< totalNumSource- 1; iSource++)
    {
        //time record
        s1= timeInputSource[iSource];
        s2= timeInputSource[iSource+ 1];
        s3= s2- s1;
        //printf("s1= %f, s2= %f\n", s1, s2);

        //coordinates data record
        x1= dataX[iSource];
        x2= dataX[iSource+ 1];
        y1= dataY[iSource];
        y2= dataY[iSource+ 1];

        //interpolation
        for(iRefer= iCount; iRefer< totalNumRefer; iRefer++)
        {
            if(timeInputRefer[iRefer]> s2)
                break;
            coe1= (timeInputRefer[iRefer]- s1)/s3;
            coe2= 1.0- coe1;
            dataInterX[iRefer]= coe1* x2+ coe2* x1;
            dataInterY[iRefer]= coe1* y2+ coe2* y1;
	    IterLocalXY.push_back(pair<double,double>(dataInterX[iRefer],dataInterY[iRefer]));
            iCount++;
        }
    }
}

