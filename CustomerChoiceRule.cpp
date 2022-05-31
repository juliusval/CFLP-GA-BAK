#include "CustomerChoiceRule.h"
#include "Params.h"
#include "LocationHelper.h"

double BinaryChoiceRule::CapturedDemand(vector<DemandPoint> demandPoints,
	vector<Facility> preexistingFacilities,
	vector<Facility> newFacilities)
{
		double U = 0;
		int bestPF;
		int bestX;
		double d;
		for (int i = 0; i < Params::DP; i++)
		{
			bestPF = 1e5;
			for (int j = 0; j < Params::PF; j++)
			{
				d = LocationHelper::DistanceToLocation(demandPoints[i].PointLocation, preexistingFacilities[j].FacilityLocation);
				if (d < bestPF) bestPF = d;
			}
			bestX = 1e5;
			for (int j = 0; j < Params::X; j++)
			{
				d = LocationHelper::DistanceToLocation(demandPoints[i].PointLocation, newFacilities[j].FacilityLocation);
				if (d < bestX)
				{
					bestX = d;
				}
			}
			if (bestX < bestPF)
			{
				U += demandPoints[i].Population;
			}
			else if (bestX == bestPF)
			{
				U += 0.3 * demandPoints[i].Population;
			}
		}
		return U;
}

double ProportionalChoiceRule::CapturedDemand(vector<DemandPoint> demandPoints,
	vector<Facility> preexistingFacilities,
	vector<Facility> newFacilities)
{
	return 0;
}