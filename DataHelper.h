#pragma once
#include <vector>
#include "Location.h"

using namespace std;

class InputPoint
{
public:
	int Id;
	Location PointLocation;
	int Population;
	int Quality;
};

class IPointDataHelper
{
public:
	virtual vector<InputPoint> FetchPoints() = 0;
};

class DataHelperFile
{
public:
	static vector<InputPoint> FetchPoints();
};