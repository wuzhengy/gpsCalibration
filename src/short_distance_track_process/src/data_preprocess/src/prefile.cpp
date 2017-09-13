#include <fstream>
#include <iostream>
#include <assert.h>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cmath>
#define IMFILELEN 256
#define IMLDLEN 1024
#define IMSDLEN 512
using namespace std;
int mergeFile(ifstream & enuFile,ifstream& weightFileName,ofstream & gpsTrackFile,double& lastTime,double& firstTime);
double checkENUfileTime(char* buf);
int main(int argc,char** argv)
{
    if(argc<4)
    {
        perror("argument error need enuOrigtemp weighttem gpstracktempglobal trackno");
        return -1;
    }
    int fileNo=atoi(argv[4]);
    int ret;
    double lastTime,firstTime,profilelastTime;
    char enuFilename[IMFILELEN];
    char weightFileName[IMFILELEN];
    ofstream gpsTrackFile;
    gpsTrackFile.open(argv[3],std::fstream::out);
    assert(gpsTrackFile.is_open());
    for(int i=1;i<=fileNo;++i)
    {
        sprintf(enuFilename,"%s%d",argv[1],i);
        ifstream enuFile(enuFilename,std::fstream::in);
        assert(enuFile.is_open());
        sprintf(weightFileName,"%s%d",argv[2],i);
        ifstream weightfile(weightFileName,std::fstream::in);
        assert(weightfile.is_open());
        ret=mergeFile(enuFile,weightfile,gpsTrackFile,lastTime,firstTime);
        profilelastTime=firstTime;
        if(i>1)
        {
            if(fabs(profilelastTime-firstTime)>0.3)
            {
                cerr<<"can not merge file because ENU file lost timeStamp"<<endl;
                exit(-1);
            }
        }
        if(ret==-1)
        {
            cerr<<"current file "<<weightFileName<<" and "<<enuFilename<<" have doesn't same length"<<endl;
            return -1;
        }
    }
    gpsTrackFile.close();
    return 0;
}
//merge gps ENU coordinate with matched weight value 
int mergeFile(ifstream & enuFile,ifstream& weightFileName,ofstream & gpsTrackFile,double &lastTime,double& firstTime)
{
    int count=0;
    char enubuf[IMLDLEN];
    char weightbuf[IMSDLEN];
    while(enuFile.getline(enubuf,IMLDLEN)&&weightFileName.getline(weightbuf,IMSDLEN))
    {
        count++;
        if(count==1){
            firstTime=checkENUfileTime(enubuf);
        }else{
            lastTime=checkENUfileTime(enubuf);
        }
        strcat(enubuf," ");
        strcat(enubuf,weightbuf);
        gpsTrackFile<<enubuf<<'\n';     
   }
   if(!enuFile.eof())
   {
        cerr<<"weight file too short"<<endl;return -1;
   }
   enuFile.close();
   weightFileName.close();
   return 0;
}
//check gps ENU coordiate piece whether legal (whether loam has lost data) this may lead track break. 
double checkENUfileTime(char* buf)
{
    string sbuf=buf;
    size_t splite=sbuf.find_last_of(" ");
    string timestr=sbuf.substr(splite+1);
    double time=atof(timestr.c_str());
    return time;
}
