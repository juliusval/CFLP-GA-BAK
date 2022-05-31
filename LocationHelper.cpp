#include <iostream>
#include <math.h>
#include "LocationHelper.h"
#include "InterfaceManager.h"
#include "Params.h"

map<pair<int, int>, double> LocationHelper::DistanceMap;

double LocationHelper::DistanceToLocation(Location fromLocation, Location toLocation)
{
	BaseAlgorithm* alg = InterfaceManager::FlpAlgorithm();
	if (alg->Distances.size() == Params::DP)
	{
		try
		{
			double distance = alg->Distances[fromLocation.ID][toLocation.ID];
			if (distance >= 0)
				return distance;
		}
		catch (const std::exception& e)
		{
			cout << "Error fetching distance" << endl;
			cerr << e.what() << endl;
		}
	}
	double dlon = fabs(fromLocation.Longitude - toLocation.Longitude);
	double dlat = fabs(fromLocation.Latitude - toLocation.Latitude);
	double aa = pow((sin((double)dlon / (double)2 * 0.01745)), 2) +
		cos(fromLocation.Longitude * 0.01745) *
		cos(toLocation.Longitude * 0.01745) *
		pow((sin((double)dlat / (double)2 * 0.01745)), 2);
	double c = 2 * atan2(sqrt(aa), sqrt(1 - aa));
	double finalVal = 6371 * c;
	return finalVal;

}