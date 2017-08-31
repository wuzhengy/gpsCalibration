#include<iostream>
#include<string.h>
#include<math.h>
#include<stdlib.h>
#include<stdio.h>
#include<map>
#define IMSIXBANDS 6.0                 //six degrees divided a band
#define IMTHREEBANDS 3.0               //three degrees divided a band

using namespace std;

const double k0 = 0.9996;

/*UTM transform*/
void UTMConversion(double latitude,double longitude,int type,double &localX,double &localY,int &bandNum)
{
	/* calcute the central meridian*/
	double meridian=0;
	if(IMTHREEBANDS == type)
	{
		if(0 == bandNum)
		{
			bandNum = longitude / IMTHREEBANDS;
			double tmp = longitude / IMTHREEBANDS;
			if((tmp - bandNum) > 0.5)
			{
				bandNum = bandNum + 1;
			}
		}
		meridian = IMTHREEBANDS * bandNum;
	}
	if(IMSIXBANDS == type)
   	{
		if(0 == bandNum)
		{
			bandNum = (int)longitude / IMSIXBANDS + 1;
		}
       	meridian = IMSIXBANDS * bandNum - IMSIXBANDS / 2;
   	}
    
	/*calcute related parameters*/
    double rB = latitude * PI / 180.0;            //angle is converted to radian
    double t = tan(rB) * tan(rB);
    double c = pow(E2,2) * pow(cos(rB),2);
    double A = (longitude - meridian) * PI / 180.0 * cos(rB);
    double N = longAxle / sqrt(1 - E1 * E1 * sin(rB) * sin(rB));
    double M = longAxle * ((1 - pow(E1,2) / 4.0 - 3.0 * pow(E1,4) / 64.0 - 5.0 * pow(E1,6) / 256.0) * rB - (3.0 * pow(E1,2) / 8.0 + 3.0 * pow(E1,4) / 32.0 + 45.0 * pow(E1,6) / 1024.0) * sin(2 * rB) + (15.0 * pow(E1,4) / 256.0 + 45.0 * pow(E1,6) / 1024.0) * sin(4 * rB) - 35.0 * pow(E1,6) / 3072.0 * sin(6 * rB));

	/*calcute the coordinate (x,y)*/
    localX = k0 * (M + N * tan(rB) * (A * A / 2.0 + (5 - t + 9 * c + 4 * c * c) * pow(A,4) / 24.0) + (61 - 58 * t + t * t + 600 * c - 330 * E2 * E2) * pow(A,6) / 720.0);
    localY=k0*N*(A+(1-t+c)*pow(A,3)/6.0+(5-18*t+t*t+72*c-58*E2*E2)*pow(A,5)/120.0)+500000;

    localY += bandNum * 10000000;
}

/*GPS coordinate transform to local coordinate*/
void UTMTransform(vector<pair<double,double> > &WGSBL,int type,vector<pair<double,double> > &localXY,double &originX,double &originY,int &originBandNum)
{
	/*
		Input: GPS coordinate
		Output: Local X and Y coordinate
		return: none
	*/
	double localX=0,localY=0;
	vector<pair<double,double> >::iterator iter=WGSBL.begin();

	/*the first coordiante is initial coordinate origin*/	
	UTMConversion(iter->first,iter->second,type,originX,originY,originBandNum);

	for(iter = WGSBL.begin();iter != WGSBL.end();iter++)
	{
		UTMConversion(iter->first,iter->second,type,localX,localY,originBandNum);
		
		localX=localX-originX;
		localY=localY-originY;
	
		localXY.push_back(pair<double,double>(localX,localY));
	}
}

void UTMReverseConversion(int type,double localX,double localY,double &latitude,double &longitude)
{
    int bandNum = localY / 10000000;         // head of y coordinate is Band number
    int meridian = 0;
    if(IMSIXBANDS == type)
    {
        meridian = IMSIXBANDS * bandNum - IMSIXBANDS / 2;
    }
    if(IMTHREEBANDS == type)
    {
        meridian = IMTHREEBANDS * bandNum;
    }
               
    localY = localY - bandNum * 10000000 - 500000;        //true y coordinate   

    /*calcute parameter*/
    double X = localX / k0;
    double fi = X / (longAxle * (1 - pow(E1,2) / 4 - 3 * pow(E1,4) / 64 - 5 * pow(E1,6) / 256));
    double e = (1 - shortAxle / longAxle) / (1 + shortAxle / longAxle);
    double Bf = fi + (3 * e / 2 - 27 * pow(e,3) / 32) * sin(2 * fi) + (21 * e * e / 16 - 55 * pow(e,4) / 32) * sin(4 * fi) + 151 * pow(e,3) / 96 * sin(6 * fi);
    double Nf = longAxle / sqrt(1 - E1 * E1 * pow(sin(Bf),2));
    double Rf = longAxle * (1 - E1 * E1) / pow((1 - E1 * E1 * pow(sin(Bf),2)),1.5);
    double D = localY / (k0 * Nf);
    double Cf = E2 * E2 *cos(Bf) * cos(Bf);
    double Tf = tan(Bf) * tan(Bf);
                                  
    /*calcute GPS coordinate*/
    latitude = Bf - Nf * tan(Bf) / Rf * (D * D / 2 - (5 + 3 * Tf + 10 * Cf - 4 * Cf * Cf - 9 * E2 * E2) * pow(D,4) / 24.0 + (61 + 90 * Tf + 298 * Cf + 45 * Tf * Tf - 252 * E2 * E2 - 3 * Cf * Cf) * pow(D,6) / 720);
    longitude = meridian + (1.0 / cos(Bf) * (D - (1 + 2 * Tf + Cf) * pow(D,3) / 6.0 + (5 - 2 * Cf + 28 * Tf - 3 * Cf * Cf + 8 * E2 * E2 + 24 * Tf * Tf) * pow(D,5) / 120.0)) * 180 / PI;

    latitude = latitude * 180 / PI;              //The radians are converted to angles
}


/*local coordinate transform to GPS coordinate*/
void UTMReverseTransform(vector<localCoordinate> ICPCoordinate,vector<pair<double,double> > &WGSBL,vector<double> &altitude,int type)
{
    double latitude=0,longitude=0;
    vector<localCoordinate>::iterator localxyz = ICPCoordinate.begin();
    for(localxyz = ICPCoordinate.begin();localxyz != ICPCoordinate.end();localxyz++)
    {
        UTMReverseConversion(type,(*localxyz).x,(*localxyz).y,latitude,longitude);
        WGSBL.push_back(pair<double,double>(latitude,longitude));
        altitude.push_back((*localxyz).z);
    }
}








