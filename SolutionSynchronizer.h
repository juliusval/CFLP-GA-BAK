#pragma once
#include "ProcessSynchronizer.h"
#include "Population.h"

using namespace std;

class SolutionSynchronizer : public ProcessSynchronizer {
public:
	virtual int Sync(Population p);
	void PrintSolutions(Population& p);
	vector<Solution> BuildFinalSolutions(Population p, vector<int> solutions);
	void LogSolutions(Population p);
	static bool sort_using_greater_than(double u, double v);
};