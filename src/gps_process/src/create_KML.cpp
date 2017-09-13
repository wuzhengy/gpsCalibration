#include <iostream>
#include <string.h>
#include <vector>
#include <fstream>
#include "parse_config.h"
#define IMSDLEN 512

int main(int argc,char **argv)
{
    /*
        argv[1]: GPS csv,the first column is longitude,the second column is latitude,the third column is height
        argv[2]: kml file
    */
	if(3 != argc)
	{
		printf("parameter error\n");
		return 1;
	}

    double latitude = 0,longitude = 0,height = 0;    // init parameter
    char buf[IMSDLEN];     // read file buffer
	char *GPSFileName = argv[1];       //  GPS csv file path
	char *KMLFileName = argv[2];        // KML file path
	vector<pair<double,double> > WGSBL;      // vector for GPS 
	vector<double> altitude;	             // vector for height

    /*open GPS csv*/
	ifstream ifile;
	ifile.open(GPSFileName);
	if(!ifile)
	{
		printf("open %s error\n",GPSFileName);
		return 1;
	}

    //read GPS csv
	while(ifile.getline(buf,IMSDLEN))
	{
		char *splite = strtok(buf," ");
		int column = 0;     
		while(splite)
		{
			column++;
            switch(column)
            {
                case 1:
                {
                    longitude = atof(splite);     // the first column is longitude
                    break;
                }
                case 2:
                {
                    latitude = atof(splite);      // the second column is latitude
                    break;
                }
                case 3:
                {
                    height = atof(splite);      // the third column is height
                    break;
                }
            }
			splite = strtok(NULL," ");
		}
		WGSBL.push_back(pair<double,double>(latitude,longitude));
		altitude.push_back(height);
	}

	ifile.close();
	
	/*create kml file*/
	if(1==createKML(KMLFileName,WGSBL,altitude))
	{
		return 1;
	}
	return 0;

}
