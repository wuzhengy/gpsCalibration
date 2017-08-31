#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<map>
#include<string.h>
#define IMSIXBANDS 6.0                     //six degrees divided a band
#define IMTHREEBANDS 3.0                   //three degrees divided a band

using namespace std;

/*calcute the Arc Length of central meridian*/
double arcLength(double latitude)
{
	double m0 = longAxle * (1 - pow(E1,2));
    double m2 = 3.0 / 2.0 * pow(E1,2) * m0;
    double m4 = 5.0 / 4.0 * pow(E1,2) * m2;
    double m6 = 7.0 / 6.0 * pow(E1,2) * m4;
    double m8 = 9.0 / 8.0 * pow(E1,2) * m6;

	double a0 = m0 + 1.0 / 2.0 * m2 + 3.0 / 8.0 * m4 + 5.0 / 16.0 * m6 + 35.0 / 128.0 * m8;
	double a2 = 1.0 / 2.0 * m2 + 1.0 / 2.0 * m4 + 15.0 / 32.0 * m6 + 7.0 / 16.0 * m8;
	double a4 = 1.0 / 8.0 * m4 + 3.0 / 16.0 * m6 + 7.0 / 32.0 * m8;
	double a6 = 1.0 / 32.0 * m6 + 1.0 / 16.0 * m8;
	double a8 = 1.0 / 128.0 * m8;
	
	double rB = latitude * PI / 180.0;                  //Angle is converted to radian
	
	double meridianLength = a0 * rB - a2 / 2.0 * sin(2 * rB) + a4 / 4.0 * sin(4 * rB) - a6 / 6.0 *sin(6 * rB) + a8 / 8.0 * sin(8 * rB);
	return meridianLength;
}

/* gaussion Transform */
void GaussionConversion(double latitude,double longitude,int type,double &localX,double &localY,int &bandNum)
{
    /* calcute the central meridian */
	double meridian = 0;
	if(IMSIXBANDS == type)
    {
		if(0 == bandNum)
		{
			bandNum = (int)longitude / IMSIXBANDS + 1;
		}
		meridian = IMSIXBANDS * bandNum - IMSIXBANDS / 2;
    }
    else if(IMTHREEBANDS == type)
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

	/*calcute related parameters*/
    double rB = latitude * PI / 180.0;        //Angle is converted to radian
    double t = tan(rB);
	double ng2 = pow(E2,2) * pow(cos(rB),2);
    double N = C / sqrt(1 + ng2);
    double m = cos(rB) * PI / 180.0 * (longitude - meridian);

    double meridianLength = arcLength(latitude);    // the length of central meridian

    /*calcute the coordinate (x,y)*/
    localX = meridianLength + N * t * (1.0 / 2.0 * m * m + 1.0 / 24.0 * (5 - t * t + 9 * ng2 + 4 * ng2 * ng2) * pow(m,4) + 1.0 / 720.0 * (61 - 58 * t * t + pow(t,4) + 270 * ng2 - 330 * ng2 * t * t) * pow(m,6));   
    localY = N * (m + 1.0 / 6.0 * (1 - t * t + ng2) * pow(m,3) + 1.0 / 120.0 * (5 - 18 * t * t + pow(t,4) + 14 * ng2 - 58 * ng2 * t * t) * pow(m,5)) + 500000;

	localY += bandNum * 10000000;           // head of y coordinate is Band number

}



/*GPS coordinate transform to local coordinate*/
void GaussionTransform(vector<pair<double,double> > WGSBL,int type,vector<pair<double,double> > &localXY,double &originX,double &originY,int &originBandNum)
{
	/*
		Input: GPS coordinate and type of dividing into bands
		Output: local X and Y coordinate
		return: none
	*/
	
	double localX=0,localY=0;
	vector<pair<double,double> >::iterator iter=WGSBL.begin();

	//the first coordinate is initial coordinate origin
	GaussionConversion(iter->first,iter->second,type,originX,originY,originBandNum);

	for(iter = WGSBL.begin();iter != WGSBL.end();iter++)
	{
		GaussionConversion(iter->first,iter->second,type,localX,localY,originBandNum);
		localX=localX-originX;
		localY=localY-originY;
		localXY.push_back(pair<double,double>(localX,localY));
	}
}

/* gaussion reverse transform  */
void GaussionReverseConversion(int type,double localX,double localY,double &latitude,double &longitude)
{
    int bandNum = localY / 10000000;
    int meridian = 0;

    /*calcute the meridian*/
    if(IMSIXBANDS == type)
    {
        meridian = IMSIXBANDS * bandNum - IMSIXBANDS / 2;
    }
    if(IMTHREEBANDS == type)
    {
        meridian = IMTHREEBANDS * bandNum;
    }

    localY = localY - bandNum * 10000000 - 500000;    //true y coordinate

    //calculate parameter
    double X = localX;
    double fi = X / (longAxle * (1 - pow(E1,2) / 4 - 3 * pow(E1,4) / 64 - 5 * pow(E1,6) / 256));
    double e = (1 - shortAxle / longAxle) / (1 + shortAxle / longAxle);
    double Bf = fi + (3 * e / 2 - 27 * pow(e,3) / 32) * sin(2 * fi) + (21 * e * e / 16 - 55 * pow(e,4) / 32) * sin(4 * fi) + 151 * pow(e,3) / 96 * sin(6 * fi);
    double Nf = longAxle / sqrt(1 - E1 * E1 * pow(sin(Bf),2));
    double Rf = longAxle * (1 - E1 * E1) / pow((1 - E1 * E1 * pow(sin(Bf),2)),1.5);
    double D = localY / (Nf);
    double Cf = E2 * E2 * cos(Bf) * cos(Bf);
    double Tf = tan(Bf) * tan(Bf);

    latitude = Bf - Nf * tan(Bf) / Rf * (D * D / 2 - (5 + 3 * Tf + Cf - 9 * Tf * Cf) * pow(D,4) / 24 + (61 + 90 * Tf + 45 * Tf * Tf) * pow(D,6) / 720);
    longitude = meridian + (1.0 / cos(Bf) * (D - (1 + 2 * Tf + Cf) * pow(D,3) / 6 + (5 + 28 * Tf + 6 * Cf + 8 * Tf * Cf + 24 * Tf * Tf) * pow(D,5) / 120)) * 180 / PI;

    latitude = latitude * 180 / PI;     //The radians are converted to angles
}

/*local coordinate transform to GPS coordinate*/
void GaussionReverseTransform(vector<localCoordinate> ICPCoordinate,vector<pair<double,double> > &WGSBL,vector<double> &altitude,int type)
{
    double latitude=0,longitude=0;
    vector<localCoordinate>::iterator localxyz;
    for(localxyz=ICPCoordinate.begin();localxyz!=ICPCoordinate.end();localxyz++)
    {
        GaussionReverseConversion(type,(*localxyz).x,(*localxyz).y,latitude,longitude);
        WGSBL.push_back(pair<double,double>(latitude,longitude));
        altitude.push_back((*localxyz).z);
    }
}

