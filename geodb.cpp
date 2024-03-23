#include "geodb.h"
#include "geotools.h"
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

GeoDatabase::GeoDatabase() {}
GeoDatabase::~GeoDatabase() {}

//Loads all the OSM map data
bool GeoDatabase::load(const std::string& map_data_file)
{
    ifstream infile(map_data_file);
    //Failed to open the file
    if (!infile)
    {
        cerr << "Error: Cannot open mapdata.txt!" << endl;
        return false;
    }
    
    string line;
    for (;;)
    {
        //Get the street name
        getline(infile, line);
        //If no street name exists, finished ==> return true
        if (!infile)
            return true;
        string street = line;
        
        //Starting coord of street segment
        string lat1; string lon1;
        //Ending coord of street segment
        string lat2; string lon2;
        infile >> lat1 >> lon1 >> lat2 >> lon2;
        
        //Get the number of points of interest
        int numPoi;
        infile >> numPoi;
        infile.ignore(10000, '\n');
        
        //Create geopoints for the starting and ending coords
        GeoPoint startC(lat1, lon1);
        GeoPoint endC(lat2, lon2);
        //Allow for bidirectional connection between startC/endC
        //Occures regardless of whether a POI exists or not
        endGeo[startC.to_string()].push_back(endC);
        endGeo[endC.to_string()].push_back(startC);
        
        //Add the street names
        string concatCoord = startC.to_string() + ":" + endC.to_string();
        string reverseConcatCoord = endC.to_string() + ":" + startC.to_string();
        streetName.insert(concatCoord, street);
        streetName.insert(reverseConcatCoord, street);
                
        //Need to account for midpoints
        if (numPoi > 0)
        {
            GeoPoint midC = midpoint(startC, endC);
            //Add the street names
            string startAndMid = startC.to_string() + ":" + midC.to_string();
            string midAndEnd = midC.to_string() + ":" + endC.to_string();
            streetName.insert(startAndMid, street);
            streetName.insert(midAndEnd, street);
            string midAndStart = midC.to_string() + ":" + startC.to_string();
            string endAndMid = endC.to_string() + ":" + midC.to_string();
            streetName.insert(midAndStart, street);
            streetName.insert(endAndMid, street);
            
            //Allow for bidirectional connection between startC/midC and midC/endC
            endGeo[startC.to_string()].push_back(midC);
            endGeo[midC.to_string()].push_back(startC);
            endGeo[midC.to_string()].push_back(endC);
            endGeo[endC.to_string()].push_back(midC);
            
            while (numPoi > 0)
            {
                getline(infile, line);
                istringstream iss(line);
                //Decrement the number of points of interest
                numPoi--;
                //Name for the point of interest
                string poiName;
                //Coord for the point of interest
                string poiLat; string poiLon;
                getline(iss, poiName, '|');
                iss >> poiLat >> poiLon;
                                
                //Create a geopoint for the poi Coord
                GeoPoint poiC(poiLat, poiLon);
                
                //Allow for bidirectional connection between poiC/midC
                endGeo[poiC.to_string()].push_back(midC);
                endGeo[midC.to_string()].push_back(poiC);
                
                //Add the street names
                string midAndPoi = midC.to_string() + ":" + poiC.to_string();
                streetName.insert(midAndPoi, "a path");
                string poiAndMid = poiC.to_string() + ":" + midC.to_string();
                streetName.insert(poiAndMid, "a path");
                
                //Add the point of interest
                pointOfInterest.insert(poiName, poiC);
            }
        }
    }
}

//Returns the geopoint where the point of interest is located
bool GeoDatabase::get_poi_location(const std::string& poi, GeoPoint& point) const
{
    const GeoPoint* temp = pointOfInterest.find(poi);
    if (temp != nullptr)
    {
        point = *temp;
        return true;
    }
    return false;
}

//Returns all geopoints that are at the other endpoint of street segments that pt is one endpoint of
std::vector<GeoPoint> GeoDatabase::get_connected_points(const GeoPoint& pt) const
{
    vector<GeoPoint> res;
    const vector<GeoPoint>* temp = endGeo.find(pt.to_string());
    for (int i = 0; i < (*temp).size(); i++)
        res.push_back((*temp)[i]);
    return res;
}

//Returns the street name associated with that segment of road
std::string GeoDatabase::get_street_name(const GeoPoint& pt1, const GeoPoint& pt2) const
{
    string oneCoord = pt1.to_string() + ":" + pt2.to_string();
    const string* temp = streetName.find(oneCoord);
    if (temp != nullptr)
        return *temp;
    return "";
}
