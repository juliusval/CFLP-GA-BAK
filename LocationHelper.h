#pragma once
#include <iostream>
#include <map>
#include "Location.h"

using namespace std;

class LocationHelper
{
public:
	static map <pair<int, int>, double> DistanceMap;
	static double DistanceToLocation(Location fromLocation, Location toLocation);
};

