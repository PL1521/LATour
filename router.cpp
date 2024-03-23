#include "router.h"
#include "geopoint.h"
#include <set>
#include <queue>
#include <algorithm>
#include "HashMap.h"
using namespace std;

Router::Router(const GeoDatabaseBase& geo_db) : m_gdb(geo_db) {}
Router::~Router() {}

//A* OR Dijkstra's algorithm
//Uncomment the temp.v line to modify it to Dijkstra
std::vector<GeoPoint> Router::route(const GeoPoint& pt1, const GeoPoint& pt2) const
{
    //Vertices to visit
    priority_queue<Pair, vector<Pair>, Compare> pq;
    //Contains visited vertices
    HashMap<GeoPoint> visited;
    //Track distances
    HashMap<double> cost;
    //Keeps track of the final path
    vector<GeoPoint> res;
    
    //If the starting geopoint matches the end geopoint, return
    if (pt1.to_string() == pt2.to_string())
    {
        res.push_back(pt1);
        return res;
    }
    
    //Push the starting vertex
    Pair p;
    p.gp = pt1;
    p.v = 0;
    pq.push(p);
    cost.insert(pt1.to_string(), 0);
    
    while (!pq.empty())
    {
        Pair curr = pq.top();
        GeoPoint currG = curr.gp;
        pq.pop();

        //If the starting geopoint is the ending geopoint, create the vector and return
        if (currG.to_string() == pt2.to_string())
        {
            res.push_back(pt2);
            GeoPoint trav = *visited.find(pt2.to_string());
            while (trav.to_string() != pt1.to_string())
            {
                res.push_back(trav);
                trav = *visited.find(trav.to_string());
            }
            res.push_back(pt1);
            //Reverse the vector
            reverse(res.begin(), res.end());
            return res;
        }
        
        //Get geopoints connected to currG
        std::vector<GeoPoint> cCoords = m_gdb.get_connected_points(currG);
        
        //Add the cost of going from the current vertex to each neighbor
        //If the new cost is less than what's currently stored in the cost hashmap, update it
        //If the neighbor doesn't exist in the distance hashmap, insert it with the new cost
        for (int i = 0; i < cCoords.size(); i++)
        {
            //if (m_gdb.get_street_name(currG, cCoords[i]) == "")
            //    continue;
            
            double newCost = *cost.find(currG.to_string()) + distance_earth_km(currG, cCoords[i]);
            if (cost.find(cCoords[i].to_string()) == nullptr || newCost < *cost.find(cCoords[i].to_string()))
            {
                cost.insert(cCoords[i].to_string(), newCost);
                Pair temp;
                temp.gp = cCoords[i];
                //temp.v = newCost; //Include this if want to use Dijkstra's instead
                temp.v = newCost + distance_earth_km(cCoords[i], pt2);
                pq.push(temp);
                visited.insert(cCoords[i].to_string(), currG);
            }
        }
    }
    return res;
}

/*
//Basic queue algorithm
//Slower and suboptimal but another method of routing
std::vector<GeoPoint> Router::route(const GeoPoint& pt1, const GeoPoint& pt2) const
{
    //Contains visited vertices
    HashMap<GeoPoint> visited;
    //Vertices to visit
    queue<GeoPoint> to_visit;
    
    //Push the starting vertex
    to_visit.push(pt1);
    
    //Keeps track of the final path
    vector<GeoPoint> res;
    
    while (!to_visit.empty())
    {
        GeoPoint curr = to_visit.front();
        to_visit.pop();
        
        //If the starting geopoint is the ending geopoint, create the vector and return
        if (curr.to_string() == pt2.to_string())
        {
            res.push_back(pt2);
            GeoPoint trav = *visited.find(pt2.to_string());
            while (trav.to_string() != pt1.to_string())
            {
                res.push_back(trav);
                trav = *visited.find(trav.to_string());
            }
            res.push_back(pt1);
        }
        
        //Get the geopoints connected to curr
        std::vector<GeoPoint> connect = m_gdb.get_connected_points(curr);
        //For each neighbor, check if it is in the visited map; if not, insert it
        //into the visited map and the to_visit map
        for (int i = 0; i < connect.size(); i++)
        {
            if (visited.find(connect[i].to_string()) == nullptr)
            {
                visited.insert(connect[i].to_string(), curr);
                to_visit.push(connect[i]);
            }
        }
    }
    return res;
}
*/
