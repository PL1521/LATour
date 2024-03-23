#include <cstdio>
#include <iomanip>
#include <iostream>
#include <map>
#include <vector>

#include "geodb.h"
#include "router.h"
#include "stops.h"
#include "tourcmd.h"
#include "tour_generator.h"

using namespace std;

void print_tour(vector<TourCommand>& tcs)
{
    double total_dist = 0;
    std::string direction;
    double street_distance = 0;

    cout << "Starting tour...\n";

    for (size_t i = 0; i < tcs.size(); ++i)
    {
        if (tcs[i].get_command_type() == TourCommand::commentary)
        {
            cout << "Welcome to " << tcs[i].get_poi() << "!\n";
            cout << tcs[i].get_commentary() << "\n";
        }
        else if (tcs[i].get_command_type() == TourCommand::turn)
        {
            cout << "Take a " << tcs[i].get_direction() << " turn on " << tcs[i].get_street() << endl;
        }
        else if (tcs[i].get_command_type() == TourCommand::proceed)
        {
            total_dist += tcs[i].get_distance();
            if (direction.empty())
                direction = tcs[i].get_direction();
            street_distance += tcs[i].get_distance();
            if (i+1 < tcs.size() && tcs[i+1].get_command_type() == TourCommand::proceed
                && tcs[i+1].get_street() == tcs[i].get_street() && tcs[i].get_street() != "a path")
            {
                continue;
            }

            cout << "Proceed " << std::fixed << std::setprecision(3) << street_distance << " miles " << direction << " on " << tcs[i].get_street() << endl;
            street_distance = 0;
            direction.clear();
        }
    }

    cout << "Your tour has finished!\n";
    cout << "Total tour distance: " << std::fixed << std::setprecision(3) << total_dist << " miles\n";
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cout << "usage: BruinTour mapdata.txt stops.txt\n";
        return 1;
    }

    GeoDatabase geodb;
    if (!geodb.load(argv[1]))
    {
        cout << "Unable to load map data: " << argv[1] << endl;
        return 1;
    }

    Router router(geodb);
    TourGenerator tg(geodb, router);

    Stops stops;
    if (!stops.load(argv[2]))
    {
        cout << "Unable to load tour data: " << argv[2] << endl;
        return 1;
    }

    std::cout << "Routing...\n\n";

    vector<TourCommand> tcs = tg.generate_tour(stops);
    if (tcs.empty())
        cout << "Unable to generate tour!\n";
    else
        print_tour(tcs);
}

/*
int main()
{
    GeoDatabase g;
    if (!g.load("mapdata.txt")) // assume this works to avoid error checking
        return 0;
    
    GeoPoint p;
    if (g.get_poi_location("1033 Gayley Avenue", p))
        cout << "The PoI is at " << p.sLatitude << ", " << p.sLongitude << endl;
    else
        cout << "PoI not found!\n";
    
    GeoPoint p1("34.0732851", "-118.4931016");
    GeoPoint p2("34.0736122", "-118.4927669");
    cout << g.get_street_name(p1, p2) << endl; // writes "Glenmere Way"
    cout << g.get_street_name(p2, p1) << endl; // writes "Glenmere Way"
    
    GeoPoint p3("34.0601422", "-118.4468929");
    GeoPoint p4("34.0600768", "-118.4467216");
    cout << g.get_street_name(p3, p4) << endl; // writes "a path"
    cout << g.get_street_name(p4, p3) << endl; // writes "a path"
    
    GeoPoint p5("34.0602175", "-118.4464952");
    GeoPoint p6("34.0600768", "-118.4467216");
    cout << g.get_street_name(p5, p6) << endl; // writes "Kinross Avenue"
    cout << g.get_street_name(p6, p5) << endl; // writes "Kinross Avenue"
    
    cout << g.get_street_name(GeoPoint("34.0666750","-118.4451224"),
                              GeoPoint("34.0664753","-118.4451537")) << endl; //Should output Westwood Plaza
    cout << g.get_street_name(GeoPoint("34.0664753","-118.4451537"),
                              GeoPoint("34.0661618","-118.4451959")) << endl; //Should output Westwood Plaza
    
    cout << endl;
    
    GeoPoint p8 = midpoint(GeoPoint("34.0581183", "-118.4587691"), GeoPoint("34.0585709","-118.4600481"));
    cout << p8.to_string() << endl;
    cout << endl;
    
    vector<GeoPoint> res1 = g.get_connected_points(GeoPoint("34.0736122", "-118.4927669"));
    for (int i = 0; i < res1.size(); i++)
        cout << res1[i].to_string() << endl;
    cout << endl;
        
    vector<GeoPoint> res2 = g.get_connected_points(GeoPoint("34.0601422", "-118.4468929"));
    for (int i = 0; i < res2.size(); i++)
        cout << res2[i].to_string() << endl;
    cout << endl;
    
    vector<GeoPoint> res3 = g.get_connected_points(GeoPoint("34.0600768", "-118.4467216"));
    for (int i = 0; i < res3.size(); i++)
        cout << res3[i].to_string() << endl;
    
    Router router(g);
    vector<GeoPoint> res = router.route(GeoPoint("34.0625329", "-118.4470263"), GeoPoint("1", "2"));
    cout << endl;
    cout << res.size() << endl;
    cout << endl;
    
    res = router.route(GeoPoint("34.0630614", "-118.4468781"), GeoPoint("34.0614911", "-118.4464410"));
    for (int i = 0; i < res.size(); i++)
        cout << res[i].to_string() << endl;
    cout << endl;
    
    res = router.route(GeoPoint("34.0614911", "-118.4464410"), GeoPoint("34.0626647", "-118.4472813"));
    for (int i = 0; i < res.size(); i++)
        cout << res[i].to_string() << endl;
}
*/
