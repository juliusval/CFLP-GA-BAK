#pragma once
#include "ProcessSynchronizer.h"
#include "Population.h"

using namespace std;

class RankSynchronizer : public ProcessSynchronizer {
public:
	virtual int Sync(Population p);
	void PrepareRankSharing(std::vector<int>& partialGrid, Population& p);
	void AssignSharedRanks(vector<int>& finalRanks, vector<Facility>& candidateFacilities);
	void LogRankSharing(std::vector<int>& ranks, std::vector<int>& finalRanks);
	void LogProcRanks(std::vector<Facility>& candidateFacilities);
	void LogFinalRanks(std::vector<int>& ranks, std::vector<int>& finalRanks);
};