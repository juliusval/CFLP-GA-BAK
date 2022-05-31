#pragma once
#include <iostream>
#include <vector>
#include <map>
#include "DemandPoint.h"
#include "Facility.h"
#include "Solution.h"

using namespace std;

class SolutionProbability
{
public:
	double SamplePosition;
	Solution SampleSolution;
};

class Population
{
public:
	map<int, double> CandidateProbabilities;
	vector<DemandPoint> DemandPoints;
	vector<Facility> PreexistingFacilities;
	vector<Facility> CandidateFacilities;

	vector<Solution> Solutions;

	double GetSolutionSamplingProbability(Solution solution);

	void BuildProbabilities(double& samplePosition, vector<SolutionProbability>& probabilities);

	void GetSampledSolution(vector<SolutionProbability>& probabilities, Solution& newSolution);

	void CalculateSamplingProbabilitiesBasedOnDistance(Solution sampledSolution, Solution newSolution, Facility facilityToChange);

	bool solutionContainsLocation(Solution solution, int facilityId);

	void UpdateRanks(Solution& oldSolution, Solution& newSolution);
	
	void UpdateRanks2();

	void AdjustWorseRanks(Solution better, Solution worse, int i);

	bool NewSolutionIsUnique(Solution newSolution);

	Solution GenerateNewSolution();
};
