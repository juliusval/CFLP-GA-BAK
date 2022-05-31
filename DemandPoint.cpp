#include "DemandPoint.h"

double DemandPoint::DistanceToPoint(Location fromLocation)
{
	return LocationHelper::DistanceToLocation(fromLocation, PointLocation);
}