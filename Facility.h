#pragma once
#include "Location.h"

class Facility
{
public:
	int Id;
	int Quality;
	int Rank;
	Location FacilityLocation;

	void setRank(int newRank);
};

