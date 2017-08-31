#ifndef __DEVIDED_SEGMENT_ICP_H__
#define __DEVIDED_SEGMENT_ICP_H__
#include <vector>
#include <string>
#include <map>

using namespace::std;
//this class entity please refer to devided_segment_icp.cpp
class devideIcp{
public:
    string findExcutableDir(string loamTrackFile);

    int callGPStoLocal(string & loamTrackFile,string & gpsTrackFile ,string & gpsENUfile);

    int callRegressICP(string & loamENUfile,string & gpsENUfile, string &coeFile, int icpType);
    
    void makeCacheDir();

    void deleteCacheDir();// you can choose delete temporary file ,here temporary file will be deleted.
private:
    string projectDir;
    string cacheDir;
	string slamMatchedGps;
};
#endif
