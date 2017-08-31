#include<iostream>
#include<string>
#include<sstream>
#include<math.h>
#include<vector>

using namespace std;

string rgbColor(double w);
int tenToSixteenString(int a, string &s);


string rgbColor(double w, double distance)
{
    float a;
    int x, y, r, g, b;

    w= w/distance;
    w= min(w/0.667, 1.0);
    a= (1- w)/0.25;
    x= floor(a);
    y= floor(255*(a-x));

    switch(x)
    {
        case 0: 
                r= 255;
                g= y;
                b= 0;
                break;
        case 1: 
                r= 255- y; 
                g= 255;
                b= 0;
                break;
        case 2: 
                r= 0;
                g= 255;
                b= y;
                break;
        case 3: 
                r= 0;
                g= 255- y;
                b= 255;
                break;
        case 4: 
                r= 0;
                g= 0;
                b= 255;
                break;
    }
    
    //cout << "r-i: " << r << endl;
    //cout << "g-i: " << g << endl;
    //cout << "b-i: " << b << endl;
    
    string rString;
    string gString;
    string bString;
    tenToSixteenString(r, rString);
    tenToSixteenString(g, gString);
    tenToSixteenString(b, bString);

    //cout << "r: " << rString<< endl;
    //cout << "g: " << gString << endl;
    //cout << "b: " << bString << endl;
    
    return "7f"+ rString+ gString+ bString;
}

int tenToSixteenString(int a, string &strTemp)
{
    char s[] = "0123456789ABCDEF";
    vector<int> v;
    vector<string> sTemp;

    v.clear();

    if (a== 0)
        v.push_back(0);

    while(a!= 0)
    {
        v.push_back(a%16);
        a= a/ 16;
    }

    vector<int>::reverse_iterator rit;
    for (rit = v.rbegin(); rit != v.rend(); rit++)
    {
        stringstream sss;
        sss << s[*rit];
        sTemp.push_back(sss.str());
    }

    if(sTemp.size()== 1)
        strTemp= "0"+ sTemp[0];
    else
        strTemp= sTemp[0]+ sTemp[1];

    return 1;
}
