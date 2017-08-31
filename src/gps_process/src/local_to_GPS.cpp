#include <iostream>
#include <fstream>
#include <vector>
#include "WGS84_parameter.h"
#include "Gaussion_transform.h"
#include "UTM_transform.h"
#include "parse_config.h"
#include "color.h"
#define IMSDLEN 512
#define IMDP 15
#define SEGMENTLEN 50

using namespace std;

/*read ENU coordinate*/
int readENUCoordinate(char *enuFileName,vector<localCoordinate> &enuCoordinate,vector<double> &enuTime)
{
    char buf[IMSDLEN];
    localCoordinate tmp;

    /*open ENU coordinate file*/
    ifstream ifile;
    ifile.open(enuFileName);
    if(NULL == ifile)
    {
        printf("open %s error\n",enuFileName);
        return 1;
    }

    /*read ENU coordinate and write into vector*/
    while(ifile.getline(buf,IMSDLEN))
    {
        int column = 0;
        char *splite = strtok(buf," ");
        while(splite)
        {
            column++;
            switch(column)
            {
                case 1:
                {
                    tmp.x = atof(splite);     // the first column is x coordinate
                    break;
                }
                case 2:
                {
                    tmp.y = atof(splite);     // the second column is y coordinate
                    break;
                }
                case 3:
                {
                    tmp.z = atof(splite);     // the third column is z coordinate
                    break;
                }
                case 4:                       // the forth column is timestamp
                {
                    enuTime.push_back(atof(splite));
                    break;
                }
                case 5:                       // the fifth column is weight
                {
                    tmp.weight = atof(splite);
                    enuCoordinate.push_back(tmp);
                    break;
                }
            }
            splite = strtok(NULL," ");
        }
    }
    ifile.close();
    return 0;
}

void segment(vector<localCoordinate> enuCoordinate,vector<pair<int,string> > &segmentColor)
{
    double distance = 0;
    double weightSum = 0;
    if(0 == enuCoordinate.size())
    {
        return;
    }
    for(int index = 1; index < enuCoordinate.size(); index ++)
    {
        double deltaX = enuCoordinate[index].x - enuCoordinate[index - 1].x;
        double deltaY = enuCoordinate[index].y - enuCoordinate[index - 1].y;
        weightSum += enuCoordinate[index].weight;
        distance += sqrt(deltaX * deltaX + deltaY * deltaY);
        if(distance > SEGMENTLEN || index == enuCoordinate.size() - 1)
        {
            //cout<<"weightSum = "<<weightSum<<endl;
            string color = rgbColor(weightSum,distance);
            segmentColor.push_back(pair<int,string>(index,color));
            distance = 0;
            weightSum = 0;
        }
    }
}

/*
//write GPS to file
int saveFile(char *GPSFileName,vector<pair<double,double> > WGSBL,vector<double> altitude,vector<double> enuTime)
{
    vector<pair<double,double> >::iterator iter;
    vector<double>::iterator iterZ,iterTime;

    //open GPS file
    ofstream ofile;
    ofile.open(GPSFileName);
    if(NULL == ofile)
    {
        printf("open %s error\n",GPSFileName);
        return 1;
    }
    ofile.precision(IMDP);
    
    for(iter = WGSBL.begin(),iterZ = altitude.begin(),iterTime = enuTime.begin();iter != WGSBL.end() && iterZ != altitude.end() && iterTime != enuTime.end(); iter++,iterZ++,iterTime++)
    {
        ofile << iter->second << ' ' << iter->first << ' ' << (*iterZ) << ' ' << (*iterTime) << endl;
    }
    ofile.close();
    return 0;
}
*/

int main(int argc,char **argv)
{
    /*
        argv[1]: enu file path
        argv[2]: transform method(Gaussion or UTM)
        argv[3]: transform type(3 or 6)
        argv[4]: 0 or 1, 0 refer to original enu file,1 refer to calibrated enu file
        argv[5]: GPS file path
    */
    if(6 != argc)
    {
        printf("parameter error\n");
        return 1;
    }

    char *enuFileName = argv[1];
    char *transformMethod = argv[2];
    int type = atoi(argv[3]);
    int flag = atoi(argv[4]);
    char *GPSKML = argv[5];

    vector<localCoordinate> ICPCoordinate;
    vector<pair<double,double> > WGSBL;
    vector<double> enuTime,altitude;
    vector<pair<int,string> > segmentColor;

    readENUCoordinate(enuFileName,ICPCoordinate,enuTime);

    if(1 == flag)
    {   
        segment(ICPCoordinate,segmentColor);
    }

    //cout<< "segmentColor = " << segmentColor.size()<<endl;
    

    if(!strcmp(transformMethod,"UTM"))
    {
        UTMReverseTransform(ICPCoordinate,WGSBL,altitude,type);
    }
    else if(!strcmp(transformMethod,"Gaussion"))
    {
        GaussionReverseTransform(ICPCoordinate,WGSBL,altitude,type);
    }
    else
    {
        printf("parameter error,the second parameter is \"UTM\" or \"Gaussion\"\t");
        return 0;
    }

    //write transformed GPS to file
    /*
    if(1==saveFile(GPSFileName,WGSBL,altitude,enuTime))
    {
        printf("write GPS failed\n");
        return 1;
    }
    */

    if(1 == flag)
    {
        createKML(GPSKML,WGSBL,altitude,segmentColor);
    }
    else
    {
        createKML(GPSKML,WGSBL,altitude);
    }
    return 0;
}
                          
