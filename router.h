#ifndef router_h
#define router_h

#include "base_classes.h"
#include "geotools.h"
#include "geodb.h"

class Router: public RouterBase
{
  public:
    Router(const GeoDatabaseBase& geo_db);
    virtual ~Router();
    virtual std::vector<GeoPoint> route(const GeoPoint& pt1, const GeoPoint& pt2) const;
  private:
    const GeoDatabaseBase& m_gdb;
    struct Pair
    {
        GeoPoint gp;
        double v;
    };
    class Compare
    {
      public:
        bool operator() (const Pair& p1, const Pair& p2)
        {
            return p1.v > p2.v;
        }
    };
};

#endif /* router_h */
