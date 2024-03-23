#ifndef tour_generator_h
#define tour_generator_h

#include "base_classes.h"
#include "geodb.h"
#include "router.h"

class TourGenerator: public TourGeneratorBase
{
  public:
    TourGenerator(const GeoDatabaseBase& geodb, const RouterBase& router);
    virtual ~TourGenerator();
    virtual std::vector<TourCommand> generate_tour(const Stops& stops) const;
  private:
    const GeoDatabaseBase& m_gd_b;
    const RouterBase& m_rb;
    std::string getdir(const GeoPoint &pt1, const GeoPoint &pt2) const;
};

#endif /* tour_generator_h */
