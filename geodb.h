#ifndef geodb_h
#define geodb_h

#include <iostream>
#include "geopoint.h"
#include "base_classes.h"
#include "HashMap.h"

class GeoDatabase: public GeoDatabaseBase
{
  public:
    GeoDatabase();
    virtual ~GeoDatabase();
    virtual bool load(const std::string& map_data_file);
    virtual bool get_poi_location(const std::string& poi, GeoPoint& point) const;
    virtual std::vector<GeoPoint> get_connected_points(const GeoPoint& pt) const;
    virtual std::string get_street_name(const GeoPoint& pt1, const GeoPoint& pt2) const;
    
  private:
    //Maps starting geopoint to ending geopoint for a segment
    HashMap<std::vector<GeoPoint>> endGeo;
    //Maps point of interest to geopoint
    HashMap<GeoPoint> pointOfInterest;
    //Maps geopoint to the street name
    HashMap<std::string> streetName;
};

#endif /* geodb_h */

