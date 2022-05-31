#include "FlpAlgorithm.h"
#include "Params.h"
#include "InterfaceManager.h"
#include "Log.h"

using namespace std;

vector<vector<Facility>> AllCombinations;
int counter = 0;

void BaseAlgorithm::PrintProgress(double progress) {
	int barWidth = 70;

	std::cout << "[";
	int pos = barWidth * progress;
	for (int i = 0; i < barWidth; ++i) {
		if (i < pos) std::cout << "=";
		else if (i == pos) std::cout << ">";
		else std::cout << " ";
	}
	std::cout << "] " << int(progress * 100.0) << " %\r";
	std::cout.flush();

	std::cout << std::endl;
}

void combinationUtil(vector<Facility> candidates, vector<Facility> newFacilities,
	int start, int end,
	int index);

void BaseAlgorithm::Init(vector<DemandPoint> dp, vector<Facility> pf, vector<Facility> cf)
{
	DemandPoints = dp;
	PreexistingFacilities = pf;
	CandidateFacilities = cf;

	for (int i = 0; i < Params::DP; i++)
	{
		vector<double> fromDPtoCL;

		for (int j = 0; j < Params::CL; j++)
		{
			double dist = LocationHelper::DistanceToLocation(DemandPoints[i].PointLocation, CandidateFacilities[j].FacilityLocation);
			fromDPtoCL.push_back(dist);
		}
		Distances.push_back(fromDPtoCL);
	}
}

Solution CompleteEnumerationAlgorithm::GetBestSolution(int argc, char* argv[])
{
	Log::WriteLine("Complete enumeration algorithm", 1);
	ValidateFacilities();

	Solution currentSolution;
	Solution bestSolution;
	vector<Facility> facilities;

	InitialFacilities(facilities);

	currentSolution.Facilities = facilities;
	bestSolution.Facilities = facilities;

	double u = InterfaceManager::CustomerChoiceRule()->CapturedDemand(DemandPoints,
		PreexistingFacilities,
		currentSolution.Facilities);
	double bestU = u;

	combinationUtil(CandidateFacilities, currentSolution.Facilities, 0, Params::CL, 0);

	for (vector<Facility> c : AllCombinations)
	{
		if (counter % 500 == 0)
		{
			PrintProgress((double)counter / (double)AllCombinations.size());
		}
		counter++;
		Solution s;
		s.Facilities = c;
		if (s.CapturedDemand(DemandPoints, PreexistingFacilities) > bestSolution.CapturedDemand(DemandPoints, PreexistingFacilities))
		{
			bestSolution = s;
		}
	}

	return bestSolution;
}

void CompleteEnumerationAlgorithm::InitialFacilities(std::vector<Facility>& facilities)
{
	for (int i = 0; i < Params::X; i++)
	{
		Facility f;
		f.FacilityLocation = CandidateFacilities[i].FacilityLocation;
		f.Id = i;
		f.Quality = CandidateFacilities[i].Quality;
		facilities.push_back(f);
	}
}

void CompleteEnumerationAlgorithm::ValidateFacilities()
{
	if (CandidateFacilities.empty() ||
		PreexistingFacilities.empty() ||
		DemandPoints.empty())
	{
		Log::WriteLine("Error: CF, PF and DP cannot be empty vectors", 1);
		exit(-1);
	}
}

void combinationUtil(vector<Facility> candidates, vector<Facility> newFacilities,
	int start, int end,
	int index)
{
	if (index == Params::X)
	{
		AllCombinations.push_back(newFacilities);
		return;
	}

	for (int i = start; i < end &&
		end - i + 1 >= Params::X - index; i++)
	{
		newFacilities[index] = candidates[i];
		combinationUtil(candidates, newFacilities, i + 1,
			end, index + 1);
	}
}