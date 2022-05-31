#pragma once
#include "DemandPoint.h"
#include "DataHelper.h"
#include "Facility.h"
#include <vector>

using namespace std;

class SettingsHelper
{
public:
	static void ReadSettings();
	static void SetInputVectors(std::vector<InputPoint>& inputPoints, std::vector<DemandPoint>& dp, std::vector<Facility>& pf, std::vector<Facility>& cf);
	static void PrintInputPoints(std::vector<InputPoint>& inputPoints);
};

