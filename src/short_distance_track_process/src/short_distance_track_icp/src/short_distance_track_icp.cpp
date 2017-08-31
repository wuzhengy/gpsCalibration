#include <iostream>
#include <vector>
#include "short_distance_track_icp.hpp"
#define IMSDLEN 512
#define BELIEVABLE 0

using namespace std;

int main(int argc,char** argv)
{
    /*
        argv[1]: slam track path
        argv[2]: local enu with weight path
        argv[3]: slam track file number
    */
    if(argc<4)
    {
        perror("argument error"); 
        exit(-1);
    }
    int loamPieceNo = atoi(argv[3]);
    char loamTrack[IMSDLEN];
    char gpsTrack[IMSDLEN];
    double weightSum = 0;         // all weight sum
    vector<int> ubTrack;          // slam track file number that not do icp yet
    vector<int> icp;              // flag to determine whether do icp or not
    TimeStruct startAndEnd;       //slam track file start time and end time

    icp.resize(loamPieceNo+1,0);     // init vector icp 0
    sprintf(gpsTrack,"%s",argv[2]);    // init gpsTrack

    for(int i = 1;i <= loamPieceNo;++i)
    {
        weightSum = 0;      //init weight
        sprintf(loamTrack,"%s%d",argv[1],i);
        getLoamTrackPieceBeginAndEndTime(loamTrack,startAndEnd);        // get loam track start time and end time
        getGPSAndWeight(gpsTrack,startAndEnd,i,weightSum);         // get gps weight
        if(BELIEVABLE < weightSum)      //if weight sum is higher than BELIEVABLE, do icp with weight using gps track and slam track
        {
            callICP(i,loamTrack,1);     // do icp using gps track and slam track
            icp[i] = 1;          
           
            // determine have slam track that not do icp yet
            if(ubTrack.empty())
            {
                continue;
            }
            else                    
            {
                if(ubTrack[0] == 1)    //if the first slam track not do icp, then do icp without weight using second slam track and first slam track 
                {
                    for(int k = ubTrack.size() - 1;k >= 0;k--)     // do the rest icp 
                    {  
                        sprintf(loamTrack,"%s%d",argv[1],ubTrack[k]);
                        callICP(ubTrack[k]+1,loamTrack,0);
                        icp[ubTrack[k]] = 1;
                    }
                }
                else
                {
                    for(int k = 0;k < ubTrack.size();k++)   // do icp
                    {
                        sprintf(loamTrack,"%s%d",argv[1],ubTrack[k]);
                        callICP(ubTrack[k]-1,loamTrack,0);
                        icp[ubTrack[k]]= 1;
                    }
                }
            }
            ubTrack.clear();

        }
        else          // weight sum is less than BELIEVABLE, push slam track file number into vector ubTrack
        {
            icp[i] = 0;
            ubTrack.push_back(i);   
        }
    }


    if(!ubTrack.empty())      // the rest slam track do icp
    {
        if(icp[ubTrack[0]-1] == 0)
        {
            cout<<"error"<<endl;
            return 0;
        }
        else
        {
            for(int k = 0;k < ubTrack.size();k++)
            {
                sprintf(loamTrack,"%s%d",argv[1],ubTrack[k]);
                callICP(ubTrack[k]-1,loamTrack,0);
                icp[ubTrack[k]] = 1;
            }
        }
    }

    ubTrack.clear();
    deleteTempFile(loamPieceNo);
    return 0;
}
