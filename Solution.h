#pragma once

#include <iostream>
#include <vector>
#include <string>
#include "DemandPoint.h"
#include "Facility.h"
#include "CustomerChoiceRule.h"

using namespace std;

class Solution
{
public:
	vector<Facility> Facilities;
	double Demand = -1;
	double CapturedDemand(vector<DemandPoint> demandPoints, vector<Facility> preexistingFacilities);
	string ToString();
};
