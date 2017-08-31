#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<fstream>
#include<vector>
#include<iomanip>
#include "WGS84_parameter.h"
#include "Gaussion_transform.h"
#include "UTM_transform.h"
#include "interpolation_by_time.h"
#define IMSDLEN 512
#define IMDP 15

using namespace std;

double originX = 0,originY = 0;   // Local original coordinate
int originBandnum = 0;       //local original coordinate band number

/*get Trail timestamp and local coordinate*/
int getTrail(char *slamTrack,vector<localCoordinate> &odometryOutput,vector<double> &trailTime,double GPSFirstTime=0,double GPSEndTime=0)
{
	/*
		Get trail Coordinate and trail timestamp between first time and end time from GPS timestamp
		the default of GPSFirstTime and GPSEngTime are 0
		
		Input: trail file path,GPSFirstTime,GPSEndTime
		Output: vector for trail timestamp,vector for trail local coordinate
		return: none
	*/
    int flag = 0;
	char buf[IMSDLEN];
    localCoordinate localcoor,originalcoor;

	/*open slam track file*/
	ifstream ifile;
	ifile.open(slamTrack);
	if(NULL == ifile)
	{
		printf("open %s error\n",slamTrack);
		return 1;
	}

	/*read trail file and get trail timestamp*/
	while(ifile.getline(buf,IMSDLEN))
	{   
		int column = 0;
		char *splite=strtok(buf," ");
		while(splite)
		{
			column++;
            switch(column)
            {
                case 1:                          // the first column is x coordinate
                {
                    localcoor.x = atof(splite);
                    break;
                }
                case 2:                          // the second column is y coordinate
                {
                    localcoor.y = atof(splite);
                    break;
                }
                case 3:                          // the third column is z coordinate
                {
                    localcoor.z = atof(splite);
                    break;
                }
                case 4:                          // the forth column is timestamp
                {
                    double timestamp = atof(splite);
                    if(0 == GPSFirstTime)
                    {
                        if(0 == GPSEndTime)          //GPSFirstTime and GPSEndTime are not set,read all slam track coordinate
                        {
                            odometryOutput.push_back(localcoor);
                            trailTime.push_back(timestamp);
                        }
                        else
                        {
                            if(timestamp < GPSEndTime)                   // GPSEndTime is set,so read line thar timestamp is less than GPSEndTime
                            {
                                odometryOutput.push_back(localcoor);
                                trailTime.push_back(timestamp);
                            }
                        }
                    }
                    else
                    {
                        if(timestamp > GPSFirstTime)      // GPSFirstTime is set,so read line that time is greater than GPSFirstTime
                        {
                            if(0 == flag)   // reset the original point
                            {
                                originalcoor = localcoor;
                                flag = 1;
                            }
                            localcoor.x -= originalcoor.x;
                            localcoor.y -= originalcoor.y;
                            localcoor.z -= originalcoor.z;
                            if((0 != GPSEndTime && timestamp < GPSEndTime) || 0 == GPSEndTime)  // if GPSEndTime is not set,read the rest line;else read line that timestamp is less than GPSEndTime
                            {
                                odometryOutput.push_back(localcoor);
                                trailTime.push_back(timestamp);
                            }
                        }
                    }
                    break;
                }
            }
			splite=strtok(NULL," ");
		}
	}
	ifile.close();
	return 0;
}

/*Get GPS Coordinate and GPS timestamp between first time and end time from trail timestamp */
int getGPS(char *originalGPS,double odometryFirstTime,double odometryEndTime,vector<pair<double,double> > &WGSBL,vector<double> &GPSTime)
{
	/*
		Input: 
			(1) GPS file path
			(2) first time from trail timestamp vector
			(3) end time from trail timestamp vector
		Output:
			(1) GPS Coordinate vector
			(2) GPS time vector
		return: none
	*/
	char buf[IMSDLEN];
    double timestamp = 0;

	/*open GPS text*/
	ifstream ifile;
	ifile.open(originalGPS);
	if(NULL == ifile)
	{
		printf("open %s error\n",originalGPS);
		return 1;
	}
    
	/*read GPS text and get GPS coordinate and GPS time*/
	while(ifile.getline(buf,IMSDLEN) && timestamp < odometryEndTime + 1)
	{
		int column = 0;
		double latitude = 90,longitude = 180;
		timestamp = 0;
		char *splite=strtok(buf,",");
		while(splite)
		{
			column++;
            if(4 == column && !strcmp("V",splite))     // the forth is 'A' or 'V'; if 'V', no GPS signal
            {
                break;
            }
            switch(column)
            {
                case 1:                    // the first column is timestamp
                {
                    timestamp = atof(splite);
                    break;
                }
                case 5:                                      // the fifth column is latitude(ddmm.mmmm)
                {
                    int tmp = atof(splite) / 100;
                    latitude = tmp + (atof(splite) - tmp * 100) / 60.0;
                    break;
                }
                case 6:                                      // the sixth column is 'N' or 'S'; if 'S',latitude is negative
                {
                    if(!strcmp("S",splite))
                    {
                        latitude = 0 - latitude;
                    }
                    break;
                }
                case 7:                                      // the seventh column is longitude(dddmm.mmmm)
                {
                    int tmp = atof(splite) / 100;
                    longitude = tmp + (atof(splite) - tmp * 100) / 60.0;
                    break;
                }
                case 8:                                      // the eighth column is 'W' or 'E'; if 'W',longitude is negative
                {
                    if(!strcmp("W",splite))
                    {
                        longitude = 0 - longitude;
                    }
                    break;
                }
            }
			splite=strtok(NULL,",");
		}
        splite = NULL;

		if((long)timestamp >= (long)(odometryFirstTime-1) && (long)time <= (long)(odometryEndTime+1))
		{
			WGSBL.push_back(pair<double,double>(latitude,longitude));
			GPSTime.push_back(timestamp);
		}
	}
	ifile.close();
	return 0;
}

/*write transformed interpolated local coordinate into file */
int saveFile(char *filename,vector<localCoordinate> enuCoordinate,vector<double> trailTime,int flag=0)
{
	/*
		Input: local coordinate vector and timestamp vector
		Output: file
		return: none
	*/
    vector<localCoordinate>::iterator iterLocalXYZ;
    vector<double>::iterator iterTime;

	/*open file*/
	ofstream ofile;
	ofile.open(filename);
	if(NULL == ofile)
	{
		printf("open %s error\n",filename);
		return 1;
	}
	ofile.precision(IMDP);	

	//write file
    if(1 == flag)
    {
	    for(iterLocalXYZ = enuCoordinate.begin(),iterTime = trailTime.begin();iterLocalXYZ != enuCoordinate.end() && iterTime != trailTime.end(); iterLocalXYZ++,iterTime++)
	    {
		    ofile << (*iterLocalXYZ).x+originX << ' ' << (*iterLocalXYZ).y+originY << ' ' << (*iterLocalXYZ).z << ' ' << (*iterTime) << endl;
	    }
    }
    else
    {
	    for(iterLocalXYZ = enuCoordinate.begin(),iterTime = trailTime.begin();iterLocalXYZ != enuCoordinate.end() && iterTime != trailTime.end(); iterLocalXYZ++,iterTime++)
	    {
		    ofile << (*iterLocalXYZ).x << ' ' << (*iterLocalXYZ).y << ' ' << (*iterLocalXYZ).z << ' ' << (*iterTime) << endl;
	    }
    }
	ofile.close();
	return 0;
}


/*gps original coordinate*/
int saveGPS(char *GPSFileName,vector<pair<double,double> > WGSBL,vector<localCoordinate> odometryOutput,vector<double> GPSTime)
{
    vector<pair<double,double> >::iterator iterBL;
    vector<double>::iterator iterTime;
    vector<localCoordinate>::iterator iterOdo;

	ofstream ofile;
	ofile.open(GPSFileName);
	if(NULL == ofile)
	{
		printf("open %s error\n",GPSFileName);
		return 1;
	}
	ofile.precision(IMDP);

	for(iterBL = WGSBL.begin()+1,iterTime = GPSTime.begin()+1,iterOdo = odometryOutput.begin()+1;iterBL != WGSBL.end()-1 && iterTime != GPSTime.end()-1 && iterOdo != odometryOutput.end()-1;iterBL++,iterTime++,iterOdo++) 
	{
		ofile << iterBL->second << ' ' << iterBL->first << ' ' << (*iterOdo).z << ' ' << (*iterTime) << endl;
	}
	ofile.close();
	return 0;
}

//timeProcess
int timeProcess(vector<double> timeOri, vector<double> &timePro, int flag)
{
    int iNum, subTime;
    double timeTemp;

    //GPS Time
    if(0== flag)
    {
        for(iNum= 0; iNum< timeOri.size(); iNum++)
        {
            if(0==iNum)
            {
                timeTemp= timeOri[iNum];
                timePro.push_back(timeTemp);
            }
            else
            {
                if(timeOri[iNum]- timePro[iNum- 1]<= 2.0)
                {
                    timeTemp= timeOri[iNum];
                    timePro.push_back(timeTemp);
                }
                    
                else
                {
                    subTime= (int)(timeOri[iNum]- timePro[iNum- 1]);
                    timeTemp= timePro[iNum- 1]+ 1.0 ;
                    timePro.push_back(timeTemp);
                }
            }
        }
    }

    return 1;
}


//gpsProcess
int gpsProcess(vector<pair<double,double> > &WGSBL,vector<double> GPSTime)
{
    int index = 0;
    while(index < WGSBL.size())
    {
        int beginIndex = -2,endIndex = -2;
        int flag = 0;

        // find gps point exception location
        for(;index < WGSBL.size(); index++)
        {
            if(90 == WGSBL[index].first && 180 == WGSBL[index].second)    // gps point exception
            {
                if(flag == 0)
                {
                    beginIndex = index - 1;      
                    flag = 1;
                }
            }
            else if(1 == flag)
            {
                endIndex = index;      
                break;
            }
        }

        if(-2 == beginIndex && -2 == endIndex)      // no gps exception
        {
            return 0;
        }

        //linear interpolation
        if(-1 == beginIndex)
        {
            if(-2 == endIndex || WGSBL.size()-1 == endIndex)        // only have one end gps point,cannot do linear interpolation
            {
                return 1;
            }
            else                                                    // linear front interpolation
            {
                double deltaT = GPSTime[endIndex + 1] - GPSTime[endIndex];                               
                double deltaB = (WGSBL[endIndex + 1].first - WGSBL[endIndex].first) / deltaT;
                double deltaL = (WGSBL[endIndex + 1].second - WGSBL[endIndex].second) / deltaT;
                for(int index = endIndex - 1;index > beginIndex; index--)\
                {
                    WGSBL[index].first = WGSBL[index + 1].first - deltaB * (GPSTime[index + 1] - GPSTime[index]);
                    WGSBL[index].second = WGSBL[index + 1].second - deltaL * (GPSTime[index + 1] - GPSTime[index]);
                }
            }
        }
        else
        {
            if(0 == beginIndex && -2 == endIndex)     // only have one begin gps point,cannot do linear interpolation
            {
                return 1;
            }
            if(0 < beginIndex && -2 == endIndex)        // linear after interpolation
            {
                double deltaT = GPSTime[beginIndex] - GPSTime[beginIndex - 1];
                double deltaB = (WGSBL[beginIndex].first - WGSBL[beginIndex - 1].first) / deltaT;
                double deltaL = (WGSBL[beginIndex].second - WGSBL[beginIndex - 1].second) / deltaT;
                for(int index = beginIndex + 1;index < WGSBL.size(); index++)
                {
                    WGSBL[index].first = WGSBL[index - 1].first + deltaB * (GPSTime[index] - GPSTime[index - 1]);
                    WGSBL[index].second = WGSBL[index - 1].second + deltaL * (GPSTime[index] - GPSTime[index - 1]);
                }
            }
            else                          // linear middle interpolation
            {
                double deltaT = GPSTime[endIndex] - GPSTime[beginIndex];
                double deltaB = (WGSBL[endIndex].first - WGSBL[beginIndex].first) / deltaT;
                double deltaL = (WGSBL[endIndex].second - WGSBL[beginIndex].second) / deltaT;
                for(int index = beginIndex + 1;index < endIndex; index++)
                {
                    WGSBL[index].first = WGSBL[index - 1].first + deltaB * (GPSTime[index] - GPSTime[index - 1]);
                    WGSBL[index].second = WGSBL[index - 1].second + deltaL * (GPSTime[index] - GPSTime[index - 1]);
                }
            }
        }
    }
    return 0;
}

int main(int argc,char **argv)
{
    /*
        argv[1]: original GPS file path (GPRMC)
        argv[2]: transform method (Gaussion or UTM) 
        argv[3]: transform type (3 or 6) 
        argv[4]: slam track (loam)
        argv[5]: save GPS from original GPS file between slam track first timestamp and end timestamp
        argv[6]: transform GPS to enu coordinate and save
        argv[7]: save slam track between GPS first timestamp and end timestamp
    */
	if(8 != argc)
	{
		printf("parameter error\n");
		return 1;
	}
	char *originalGPS = argv[1];
	char *transformMethod = argv[2];
	int type = atoi(argv[3]);
	char *slamOdometry = argv[4];
	char *GPSFileName = argv[5];
	char *enuCoorFileName = argv[6];
	char *slamFinalOdometry = argv[7];

    double odometryFirstTime = 0,odometryEndTime = 0,
           GPSFirstTime = 0,GPSEndTime = 0;

	vector<pair<double,double> > WGSBL,localXY,iterLocalXY;
	vector<double> GPSTimeTemp, GPSTime, odometryTime, odometryTimeTemp;
	vector<localCoordinate> odometryOutput,enuCoordinate;
	vector<pair<double,double> >::iterator iterlocalxy;
	vector<double>::iterator iter;
	vector<localCoordinate>::iterator iterlocalxyz;
	
	/* get the slam timestamp */
	if(1 == getTrail(slamOdometry,odometryOutput,odometryTimeTemp))
	{
		return 1;
	}

    //OdometryTime Process
    timeProcess(odometryTimeTemp, odometryTime, 0);
	
    iter = odometryTime.begin();
	odometryFirstTime = (*iter);           //get slam track start time

	iter = odometryTime.end()-1;
	odometryEndTime = (*iter);             //get slam track end time

	/*Get the GPS for a given time period*/
    if(1 == getGPS(originalGPS,odometryFirstTime,odometryEndTime,WGSBL,GPSTimeTemp))
	{
		return 1;
	}

    //GPSTime Process
    timeProcess(GPSTimeTemp, GPSTime, 0);

	if(0 == WGSBL.size())
	{
		printf("no GPS information for this time period\n");
		return 1;
	}

	iter = GPSTime.begin();
	GPSFirstTime = (*iter);          //get GPS start time

	iter = GPSTime.end()-1;
	GPSEndTime = (*iter);            //get GPS end time


	if(GPSFirstTime > odometryFirstTime)
	{
		printf("GPS started from %lf,but Lidar started from %lf,so we should start getting Lidar information closest to GPS First time %lf\n",GPSFirstTime,odometryFirstTime,GPSFirstTime);
        cout << "getTrail2" << endl;
        odometryOutput.clear();
        odometryTime.clear();
		if(1 == getTrail(slamOdometry,odometryOutput,odometryTime,GPSFirstTime))
		{
			return 1;
		}
	}

	if(GPSEndTime < odometryEndTime)
	{
		printf("GPS ended to %lf,but Lidar ended to %lf,so we should end getting Lidar information closest to GPS End time %lf\n",GPSEndTime,odometryEndTime,GPSEndTime);
		cout<<"getTrail3"<<endl;
        odometryOutput.clear();
        odometryTime.clear();
        if(1 == getTrail(slamOdometry,odometryOutput,odometryTime,GPSFirstTime,GPSEndTime))
		{
			return 1;
		}
	}

    if(1 == gpsProcess(WGSBL,GPSTime))
    {
        cout<<"GPS error"<<endl;
        return 1;
    }

	if(1 == saveGPS(GPSFileName,WGSBL,odometryOutput,GPSTime))
	{
		return 1;
	}

    //cout<<WGSBL.size()<<endl;
	/*UTM transfom with 3 degree a band */
	if(!strcmp(transformMethod,"UTM"))
	{
		//UTMTransform(WGSBL,type,LocalXY);
		UTMTransform(WGSBL,type,localXY,originX,originY,originBandnum);
		//printf("originX=%lf  originY=%lf\n",originX,originY);
	}
	else if(!strcmp(transformMethod,"Gaussion"))
	{
		//GaussionTransform(WGSBL,type,LocalXY);
		GaussionTransform(WGSBL,type,localXY,originX,originY,originBandnum);
		//printf("originX=%lf originY=%lf\n",originX,originY);
	}
	else
	{
		printf("parameter error, the second parameter is \"UTM\" or \"Gaussion\"\n");
		return 0;
	}

	/*Do the interpolation*/
	interPolate(localXY,GPSTime,odometryTime,iterLocalXY);

	iterlocalxy = iterLocalXY.begin();

	double tmpX = iterlocalxy->first;
	double tmpY = iterlocalxy->second;

	originX += tmpX;
	originY += tmpY;
	
	for(iterlocalxy = iterLocalXY.begin(),iterlocalxyz = odometryOutput.begin();iterlocalxy != iterLocalXY.end() && iterlocalxyz!=odometryOutput.end();iterlocalxy++,iterlocalxyz++)
	{
		localCoordinate tmp;
		tmp.x = (iterlocalxy->first) - tmpX;
		tmp.y = (iterlocalxy->second) - tmpY;
		tmp.z = (*iterlocalxyz).z;
		enuCoordinate.push_back(tmp);
	}

	if(1==saveFile(slamFinalOdometry,odometryOutput,odometryTime))
	{
		return 1;
	}

	/* write result into file*/
	if(1==saveFile(enuCoorFileName,enuCoordinate,odometryTime,1))
	{
		return 1;
	}
	return 0;
}
