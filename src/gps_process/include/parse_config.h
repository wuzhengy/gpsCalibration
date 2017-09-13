#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/parser.h>
#include <libxml/xmlmemory.h>
#include <libxml/xmlstring.h>
#define IMDP 15

using namespace std;

vector<string> configParameter;

int readParameter();

int createKML(char *KMLFileName,vector<pair<double,double> > WGSBL,vector<double> altitude)
{
    if(1==readParameter())
    {
        return 1;
    }

    ofstream ofile;
    ofile.open(KMLFileName);
    if(!ofile)
    {
        printf("open %s error\n",KMLFileName);
        return 1;
    }
    ofile.precision(IMDP);
   
    int index = 0;
	ofile<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"<<endl;
	ofile<<"<kml xmlns=\"http://www.opengis.net/kml/2.2\">"<<endl;
	ofile<<"<Document>"<<endl;
	ofile<<"<name>"<<"original GPS"<<"</name>"<<endl;
	ofile<<"<description>"<<"original GPS"<<"</description>"<<endl;

    ofile<<"<Style id=\""<<configParameter[index++]<<"\">"<<endl;
	ofile<<"<LineStyle>"<<endl;
	ofile<<"<color>"<<"7fFF00FF"<<"</color>"<<endl;
	ofile<<"<width>"<<configParameter[index++]<<"</width>"<<endl;
	ofile<<"</LineStyle>"<<endl;
	ofile<<"<PolyStyle>"<<endl;
	ofile<<"<color>"<<"7fFF00FF"<<"</color>"<<endl;
	ofile<<"</PolyStyle>"<<endl;
	ofile<<"</Style>"<<endl;
	ofile<<"<Placemark>"<<endl;
	ofile<<"<styleUrl>"<<configParameter[index++]<<"</styleUrl>"<<endl;
	ofile<<"<LineString>"<<endl;
	ofile<<"<extrude>"<<configParameter[index++]<<"</extrude>"<<endl;
	ofile<<"<tessellate>"<<configParameter[index++]<<"</tessellate>"<<endl;
	ofile<<"<altitudeMode>"<<configParameter[index++]<<"</altitudeMode>"<<endl;
	ofile<<"<coordinates>"<<endl;

    for(int i = 0;i < WGSBL.size() && i < altitude.size(); i++)
    {
        ofile<<WGSBL[i].second<<','<<WGSBL[i].first<<','<<altitude[i]<<endl;
    }

    ofile<<"</coordinates>"<<endl;
    ofile<<"</LineString></Placemark>"<<endl;
	ofile<<"</Document></kml>"<<endl;
    return 0;
}

int createKML(char *KMLFileName,vector<pair<double,double> > WGSBL,vector<double> altitude,vector<pair<int,string> > segmentColor)
{
    int index = 0;     

	if(1==readParameter())
	{
		return 1;
	}

	/*open kml file*/
	ofstream ofile;
	ofile.open(KMLFileName);
	if(NULL==ofile)
	{
		printf("open %s error\n",KMLFileName);
		return 1;
	}
	ofile.precision(IMDP);

	ofile<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"<<endl;
	ofile<<"<kml xmlns=\"http://www.opengis.net/kml/2.2\">"<<endl;
	ofile<<"<Document>"<<endl;
	ofile<<"<name>"<<"calibrated GPS"<<"</name>"<<endl;
	ofile<<"<description>"<<"calibrated GPS"<<"</description>"<<endl;
	
    int indexCoor = 0;

    for(int i = 0;i < segmentColor.size(); i++)
    {
        index = 0;
        ofile<<"<Style id=\""<<configParameter[index++]<<"\">"<<endl;
	    ofile<<"<LineStyle>"<<endl;
	    ofile<<"<color>"<<segmentColor[i].second<<"</color>"<<endl;
	    ofile<<"<width>"<<configParameter[index++]<<"</width>"<<endl;
	    ofile<<"</LineStyle>"<<endl;
	    ofile<<"<PolyStyle>"<<endl;
	    ofile<<"<color>"<<segmentColor[i].second<<"</color>"<<endl;
	    ofile<<"</PolyStyle>"<<endl;
	    ofile<<"</Style>"<<endl;
	    ofile<<"<Placemark>"<<endl;
	    ofile<<"<styleUrl>"<<configParameter[index++]<<"</styleUrl>"<<endl;
	    ofile<<"<LineString>"<<endl;
	    ofile<<"<extrude>"<<configParameter[index++]<<"</extrude>"<<endl;
	    ofile<<"<tessellate>"<<configParameter[index++]<<"</tessellate>"<<endl;
	    ofile<<"<altitudeMode>"<<configParameter[index++]<<"</altitudeMode>"<<endl;
	    ofile<<"<coordinates>"<<endl;
	
        for(;indexCoor < segmentColor[i].first && index < altitude.size(); indexCoor ++)
        {
            ofile<<WGSBL[indexCoor].second<<','<<WGSBL[indexCoor].first<<','<<altitude[indexCoor]<<endl;
        }
        ofile<<"</coordinates>"<<endl;
        ofile<<"</LineString></Placemark>"<<endl;
    }

	ofile<<"</Document></kml>"<<endl;

    ofile.close();
	return 0;
}

/*read parameter from kml_config.xml*/
int readParameter()
{
	xmlDocPtr pDoc=xmlReadFile("./src/gps_process/config/kml_config.xml","UTF-8",XML_PARSE_RECOVER); //get xml pointer
	if(NULL==pDoc)
	{
		printf("open config.xml error\n");
		return 1;
	}

	xmlNodePtr pRoot=xmlDocGetRootElement(pDoc);     //get xml root

	if(NULL==pRoot)
	{
		printf("get config.xml root error\n");
		return 1;
	}

	xmlNodePtr pFirst=pRoot->children;	

	while(NULL!=pFirst)
	{
		xmlChar *value=NULL;	
		if(!xmlStrcmp(pFirst->name,(const xmlChar *)("style")))
		{
			xmlNodePtr pStyle=pFirst->children;
			while(NULL!=pStyle)
			{
				value=xmlNodeGetContent(pStyle);
				if(xmlStrcmp(pStyle->name,(const xmlChar *)("text")))
				{
                    configParameter.push_back((char *)value);
				}
				pStyle=pStyle->next;
			}
		}
		else if(!xmlStrcmp(pFirst->name,(const xmlChar *)("Placemark")))
		{
			xmlNodePtr pPlacemark=pFirst->children;
			while(NULL!=pPlacemark)
			{
				value=xmlNodeGetContent(pPlacemark);
				if(xmlStrcmp(pPlacemark->name,(const xmlChar *)("text")))
				{
                    configParameter.push_back((char *)value);
				}
				pPlacemark=pPlacemark->next;
			}
		}
		else
		{
			value=xmlNodeGetContent(pFirst);
			if(xmlStrcmp(pFirst->name,(const xmlChar *)("text")))
			{
                configParameter.push_back((char *)value);
			}
		}
		pFirst=pFirst->next;
	}
	return 0;
}
