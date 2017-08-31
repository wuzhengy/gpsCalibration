#include<iostream>
#include<math.h>
#define PI 3.141592653589

/*WGS84 parameter*/	
const double longAxle=6378137;
const double shortAxle=6356752.314;
const double E1=sqrt(longAxle*longAxle-shortAxle*shortAxle)/longAxle;
const double E2=sqrt(longAxle*longAxle-shortAxle*shortAxle)/shortAxle;	
const double C=longAxle*longAxle/shortAxle;

struct localCoordinate
{
	double x;
	double y;
	double z;
    double weight;
};
