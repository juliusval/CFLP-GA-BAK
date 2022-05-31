#include <iostream>
#include <string>
#include <sstream>
#include "Solution.h"
#include "InterfaceManager.h"
#include "Params.h"

using namespace std;

double Solution::CapturedDemand(vector<DemandPoint> demandPoints, vector<Facility> preexistingFacilities)
{
	if(Solution::Facilities.size() < Params::X)
	{
		return -2;
	}
	if (Solution::Demand == -1)
	{
		Solution::Demand = InterfaceManager::CustomerChoiceRule()->CapturedDemand(demandPoints, preexistingFacilities, Solution::Facilities);
	}
	return Solution::Demand;
}

string Solution::ToString()
{
	stringstream ss;
	ss << "Facilities: ";
	for (Facility f : Facilities)
	{
		ss << " " << f.Id;
	}
	ss << " Captured demand: " << Demand << endl;
	return ss.str();
}
