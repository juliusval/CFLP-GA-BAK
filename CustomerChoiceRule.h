#pragma once
#include <vector>
#include "DemandPoint.h"
#include "Facility.h"

using namespace std;

class ICustomerChoiceRule
{
public:
	virtual ~ICustomerChoiceRule() {}
	virtual double CapturedDemand(vector<DemandPoint> demandPoints,
		vector<Facility> preexistingFacilities,
		vector<Facility> newFacilities) = 0;
};

class BinaryChoiceRule : public ICustomerChoiceRule
{
public:
	virtual double CapturedDemand(vector<DemandPoint> demandPoints,
		vector<Facility> preexistingFacilities,
		vector<Facility> newFacilities);
};

class ProportionalChoiceRule : public ICustomerChoiceRule
{
public:
	virtual double CapturedDemand(vector<DemandPoint> demandPoints,
		vector<Facility> preexistingFacilities,
		vector<Facility> newFacilities);
};