#include "tour_generator.h"
#include "tourcmd.h"
#include "stops.h"
using namespace std;

TourGenerator::TourGenerator(const GeoDatabaseBase& geodb, const RouterBase& router)
: m_gd_b(geodb), m_rb(router) {}

TourGenerator::~TourGenerator() {}

std::vector<TourCommand> TourGenerator::generate_tour(const Stops& stops) const
{
    std::vector<TourCommand> res;
    std::vector<TourCommand> tempRes;
    
    //Represents current poi's name and description
    std::string pName;
    std::string pDescript;
    //Represents next poi's name and description
    std::string nextName;
    std::string nextDescript;
    for (int i = 0; i < stops.size() - 1; i++)
    {
        //Get the curr poi's name and description
        stops.get_poi_data(i, pName, pDescript);
        TourCommand t;
        t.init_commentary(pName, pDescript);
        tempRes.push_back(t);
        GeoPoint poi1;
        //If the poi doesn't exist, return the empty vector
        if (!m_gd_b.get_poi_location(pName, poi1))
            return res;
        
        //Get the next poi's name and description
        stops.get_poi_data(i + 1, nextName, nextDescript);
        GeoPoint poi2;
        //If the poi doesn't exist, return the empty vetor
        if (!m_gd_b.get_poi_location(nextName, poi2))
            return res;
        
        std::vector<GeoPoint> currToNext = m_rb.route(poi1, poi2);
        //If no route exists, return the empty vector
        if (currToNext.size() == 0)
            return res;
        
        string prevStrAngle;
        string prevStreet;
        string currStrAngle;
        string currStreet;
        string nextStrAngle;
        string nextStreet;
        GeoPoint prevG;
        GeoPoint currG;
        GeoPoint nextG;
        GeoPoint nextNextG;
        for (int j = 0; j < currToNext.size() - 2; j++)
        {
            currG = currToNext[j];
            nextG = currToNext[j + 1];
            nextNextG = currToNext[j + 2];
            
            //Calculate the angle between the current and next geopoint
            double currAngle = angle_of_line(currG, nextG);
            if (currAngle >= 0 && currAngle < 22.5)
                currStrAngle = "east";
            else if (currAngle < 67.5)
                currStrAngle = "northeast";
            else if (currAngle < 112.5)
                currStrAngle = "north";
            else if (currAngle < 157.5)
                currStrAngle = "northwest";
            else if (currAngle < 202.5)
                currStrAngle = "west";
            else if (currAngle < 247.5)
                currStrAngle = "southwest";
            else if (currAngle < 292.5)
                currStrAngle = "south";
            else if (currAngle < 337.5)
                currStrAngle = "southeast";
            else
                currStrAngle = "east";
            
            //Get the street name
            currStreet = m_gd_b.get_street_name(currG, nextG);
            
            //Add a proceed tour command for the two geopoints
            TourCommand temp;
            temp.init_proceed(currStrAngle, currStreet,
                              distance_earth_miles(currG, nextG), currG, nextG);
            tempRes.push_back(temp);
            
            //Calculate the angle between the next geopoint and the one after it
            double nextAngle = angle_of_line(nextG, nextNextG);
            if (nextAngle >= 0 && nextAngle < 22.5)
                nextStrAngle = "east";
            else if (nextAngle < 67.5)
                nextStrAngle = "northeast";
            else if (nextAngle < 112.5)
                nextStrAngle = "north";
            else if (nextAngle < 157.5)
                nextStrAngle = "northwest";
            else if (nextAngle < 202.5)
                nextStrAngle = "west";
            else if (nextAngle < 247.5)
                nextStrAngle = "southwest";
            else if (nextAngle < 292.5)
                nextStrAngle = "south";
            else if (nextAngle < 337.5)
                nextStrAngle = "southeast";
            else
                nextStrAngle = "east";
            
            //Get the street name of the next street
            nextStreet = m_gd_b.get_street_name(nextG, nextNextG);
            
            //Add a turn tour command if street names AND angles differ
            if (currStreet != nextStreet)
            {
                //Calculate the angle to turn
                double turnAngle = angle_of_turn(currG, nextG, nextNextG);
                std::string turnStrAngle;
                if (turnAngle >= 1 && turnAngle < 180)
                    turnStrAngle = "left";
                else if (turnAngle >= 180 && turnAngle <= 359)
                    turnStrAngle = "right";
                if (turnStrAngle != "")
                {
                    TourCommand turnC;
                    turnC.init_turn(turnStrAngle, nextStreet);
                    tempRes.push_back(turnC);
                }
            }
            prevStreet = currStreet;
            prevStrAngle = currStrAngle;
            prevG = currG;
        }
        //Add the last proceed
        TourCommand lastP;
        lastP.init_proceed(nextStrAngle, nextStreet,
                          distance_earth_miles(nextG, nextNextG), nextG, nextNextG);
        tempRes.push_back(lastP);
    }
    //Add the last point of interest
    TourCommand lastC;
    if (stops.size() == 1)
        stops.get_poi_data(0, nextName, nextDescript);
    lastC.init_commentary(nextName, nextDescript);
    tempRes.push_back(lastC);
    res = tempRes;
    return res;
}

