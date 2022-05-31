#pragma once

#include <iostream>
#include <vector>
#include <set>
#include "FlpAlgorithm.h"
#include "Solution.h"
#include "DemandPoint.h"
#include "Facility.h"
#include "Population.h"

using namespace std;

class GAsolutionDb : public BaseAlgorithm {
public:
	vector<Solution> SolutionDatabase;
	set<vector<int>> SolutionSet;
	virtual Solution GetBestSolution(int argc, char* argv[]);
	Solution LaunchMasterDb(Population p);
	void InitParams(Population& p);
	void HandleBestSolutionFound(Population& p, vector<int> procData);
	void PrintSolutionDatabase(Population p);
	vector<int> SolutionDatabaseRanks();
	vector<int> SolutionDatabaseRanksWithAdjustment();
	void AdjustRanksFromSolutionDb(vector<int> rankOffsets, Population& p);
};
