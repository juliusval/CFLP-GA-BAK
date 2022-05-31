#pragma once
#include "Location.h"
#include "LocationHelper.h"

class DemandPoint
{
public:
	int Population;
	Location PointLocation;
	double DistanceToPoint(Location fromLocation);
};