#pragma once

#include <iostream>
#include <vector>
#include "Solution.h"
#include "Run.h"

using namespace std;

class ResultHelper
{
public:
	static double AverageDemand(vector<Run> runs);
	static double MaxDemand(vector<Run> runs);
	static double StandardDeviation(vector<Run> runs);
	static double RoundResult(double val);
	static int BestRun(vector<Run> runs);
};