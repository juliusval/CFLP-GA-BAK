#pragma once

#include <iostream>
#include <vector>
#include "Solution.h"
#include "DemandPoint.h"
#include "Facility.h"

using namespace std;

class BaseAlgorithm
{
public:
	virtual ~BaseAlgorithm() {};

	vector<vector<double>> Distances;
	vector<DemandPoint> DemandPoints;
	vector<Facility> PreexistingFacilities;
	vector<Facility> CandidateFacilities;
	vector<double> DemandProgess;

	void PrintProgress(double progress);

	void Init(vector<DemandPoint> dp, vector<Facility> pf, vector<Facility> cf);

	virtual Solution GetBestSolution(int argc, char* argv[]) = 0;
};

class CompleteEnumerationAlgorithm : public BaseAlgorithm {
public:
	virtual Solution GetBestSolution(int argc, char* argv[]);
	void InitialFacilities(std::vector<Facility>& facilities);
	void ValidateFacilities();

};

class PopAlgorithm : public BaseAlgorithm {
public:
	virtual Solution GetBestSolution(int argc, char* argv[]);
};
