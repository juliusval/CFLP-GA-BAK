#include <iostream>
#include <map>
#include <set>
#include <algorithm>
#include <vector>
#include <climits>
#include "Population.h"
#include "Params.h"
#include "DemandPoint.h"
#include "Facility.h"
#include "Solution.h"
#include "Log.h"

using namespace std;


void Population::BuildProbabilities(double& samplePosition, vector<SolutionProbability>& probabilities)
{
	double sumDemand = 0;
	for (Solution s : Solutions)
	{
		sumDemand += s.CapturedDemand(DemandPoints, PreexistingFacilities);
	}

	for (Solution s : Solutions)
	{
		double samplingProb = s.CapturedDemand(DemandPoints, PreexistingFacilities) / sumDemand;
		samplePosition += samplingProb;
		SolutionProbability sp;
		sp.SamplePosition = samplePosition;
		sp.SampleSolution = s;
		probabilities.push_back(sp);
	}
	probabilities[probabilities.size() - 1].SamplePosition = 1.0;
}

void Population::GetSampledSolution(vector<SolutionProbability>& probabilities, Solution& newSolution)
{
	double randomPosition = ((double)rand() / RAND_MAX);
	for (int i = 0; i < probabilities.size(); i++)
	{
		if (i == 0)
		{
			if (probabilities[i].SamplePosition >= randomPosition)
			{
				newSolution = probabilities[i].SampleSolution;
			}
		}
		else
		{
			if (probabilities[i - 1].SamplePosition < randomPosition &&
				probabilities[i].SamplePosition >= randomPosition)
			{
				newSolution = probabilities[i].SampleSolution;
			}
		}
	}
}

void Population::CalculateSamplingProbabilitiesBasedOnDistance(Solution sampledSolution, Solution newSolution, Facility facilityToChange)
{
	double probabilitySum = 0;
	double rankDistanceRatioSum = 0;
	int minRank = INT_MAX;
	double minDistance = (double)INT_MAX;
	int maxRank = INT_MIN;
	double maxDistance = (double)INT_MIN;
	double* distancesFromXkToCandidate = new double[Params::CL];
	double eps = 1;
	for (int i = 0; i < Params::CL; i++)
	{
		if (solutionContainsLocation(sampledSolution, i) || solutionContainsLocation(newSolution, i) || i == facilityToChange.Id)
			continue;

		if (CandidateFacilities[i].Rank < minRank)
		{
			minRank = CandidateFacilities[i].Rank;
		}
		else if (CandidateFacilities[i].Rank > maxRank)
		{
			maxRank = CandidateFacilities[i].Rank;
		}

		double distanceFromXkToCandidate = LocationHelper::DistanceToLocation(facilityToChange.FacilityLocation,
			CandidateFacilities[i].FacilityLocation);
		distancesFromXkToCandidate[i] = distanceFromXkToCandidate;

		if (distanceFromXkToCandidate < minDistance)
		{
			minDistance = distanceFromXkToCandidate;
		}
		else if (distanceFromXkToCandidate > maxDistance)
		{
			maxDistance = distanceFromXkToCandidate;
		}
	}

	CandidateProbabilities.clear();
	for (int i = 0; i < Params::CL; i++)
	{
		if (solutionContainsLocation(sampledSolution, i) || solutionContainsLocation(newSolution, i) || i == facilityToChange.Id)
			continue;
		CandidateProbabilities.insert(pair<int, double>(i, 0));
		double normalizedRank = (double)((CandidateFacilities[i].Rank - minRank) + eps) / ((maxRank - minRank) + eps);
		double normalizedDistance = ((distancesFromXkToCandidate[i] - minDistance) + eps) / ((maxDistance - minDistance) + eps);
		rankDistanceRatioSum += normalizedRank / normalizedDistance;
	}

	for (auto i = CandidateProbabilities.begin(); i != CandidateProbabilities.end(); i++)
	{
		double normalizedRank = (double)((CandidateFacilities[i->first].Rank - minRank) + eps) / ((double)(maxRank - minRank) + eps);
		double normalizedDistance = ((distancesFromXkToCandidate[i->first] - minDistance) + eps) / ((maxDistance - minDistance) + eps);
		probabilitySum += (normalizedRank / normalizedDistance) / rankDistanceRatioSum;
		i->second = probabilitySum;
		auto a = i;
		if ((++a) == CandidateProbabilities.end())
		{
			i->second = 1.0;
		}
	}
	delete distancesFromXkToCandidate;
}

bool Population::solutionContainsLocation(Solution solution, int facilityId)
{
	for (Facility f : solution.Facilities)
	{
		if (f.Id == facilityId)
			return true;
	}
	return false;
}

void Population::UpdateRanks(Solution& oldSolution, Solution& newSolution)
{
	if (newSolution.CapturedDemand(DemandPoints, PreexistingFacilities) >
		oldSolution.CapturedDemand(DemandPoints, PreexistingFacilities))
	{
		for (int i = 0; i < Params::X; i++)
		{
			CandidateFacilities[newSolution.Facilities[i].Id].Rank++;
			AdjustWorseRanks(newSolution, oldSolution, i);
		}

		for (int i = 0; i < Params::X; i++)
			oldSolution.Facilities[i] = newSolution.Facilities[i];
	}
	else
	{
		for (int i = 0; i < Params::X; i++)
		{
			AdjustWorseRanks(oldSolution, newSolution, i);
		}
	}
}

void Population::UpdateRanks2()
{
	for (Solution &s : Solutions)
	{
		for (Facility& f : s.Facilities)
		{
			for (Facility& fc : CandidateFacilities)
			{
				if (fc.Id == f.Id)
				{
					fc.Rank++;
				}
			}
		}
	}
}

void Population::AdjustWorseRanks(Solution better, Solution worse, int i)
{
	if (!solutionContainsLocation(better, worse.Facilities[i].Id))
	{
		CandidateFacilities[worse.Facilities[i].Id].Rank--;
		if (CandidateFacilities[worse.Facilities[i].Id].Rank == 0)
		{
			for (Facility& f : CandidateFacilities)
			{
				f.Rank++;
			}
		}
	}
}

bool CompareSolutions(const Solution& s1, const Solution& s2)
{
	return s1.Demand > s2.Demand;
}

Solution Population::GenerateNewSolution()
{
	if (Solutions.empty())
	{
		Solution s;
		int counter = 0;
		for (int i = 0; i < Params::X; i++)
		{
			s.Facilities.push_back(CandidateFacilities[i]);
		}
		s.CapturedDemand(DemandPoints, PreexistingFacilities);
		Solutions.push_back(s);
		return s;
	}

	double samplePosition = 0;
	Solution sampledSolution;
	Solution newSolution;
	vector<SolutionProbability> solutionProbabilities;

	BuildProbabilities(samplePosition, solutionProbabilities);
	GetSampledSolution(solutionProbabilities, sampledSolution);

	solutionProbabilities.clear();
	bool newSolutionMutated = false;

	bool mustBeChanged = false;
	for (Facility f : sampledSolution.Facilities)
	{
		double random = ((double)rand() / RAND_MAX);
		double probToChange = 1.0 / (double)Params::X;
		if (random < probToChange)
		{
			mustBeChanged = true;
			// try
			// {
			// 	UpdateRanks2();
			// }
			// catch (const std::exception&)
			// {
			// 	Log::WriteLine("Update Ranks 2 failed", 0);
			// }

			try
			{
				CalculateSamplingProbabilitiesBasedOnDistance(sampledSolution, newSolution, f);
			}
			catch (const std::exception&)
			{
				Log::WriteLine("Sampling probability calculation failed", 0);
			}


			try
			{
				clock_t begin_time = clock();
				double candidateLocationRandValue = ((double)rand() / RAND_MAX);

				for (auto it = CandidateProbabilities.begin(); it != CandidateProbabilities.end(); it++)
				{
					if (candidateLocationRandValue <= it->second)
					{
						Facility newFacility;
						newFacility.Id = it->first;
						newFacility.Quality = CandidateFacilities[it->first].Quality;
						newFacility.Rank = CandidateFacilities[it->first].Rank;
						newFacility.FacilityLocation = CandidateFacilities[it->first].FacilityLocation;

						newSolution.Facilities.push_back(newFacility);
						newSolutionMutated = true;
						break;
					}
				}
			}
			catch (const std::exception&)
			{
				Log::WriteLine("New facility generation failed having cand. probs.", 0);
			}
		}
		else
		{
			newSolution.Facilities.push_back(f);
		}
	}

	bool isUnique = NewSolutionIsUnique(newSolution);
	
	if (newSolutionMutated && isUnique)
	{
		UpdateRanks(sampledSolution, newSolution);

		newSolution.CapturedDemand(DemandPoints, PreexistingFacilities);

		Solutions.push_back(newSolution);
		std::sort(Solutions.begin(), Solutions.end(), CompareSolutions);

		if (Solutions.size() > Params::MAX_POPULATION_SIZE)
		{
			/*Solution lastSolution;
			lastSolution = Solutions[Solutions.size() - 1];
			for (int i = 0; i < Params::X; i++)
			{
				delete lastSolution.Facilities[i];
			}
			delete lastSolution;*/
			Solutions.pop_back();
		}

	}
	return newSolution;
}

bool Population::NewSolutionIsUnique(Solution newSolution)
{
	if (newSolution.Facilities.size() < Params::X)
	{
		Log::WriteLine("newSolution.Facilities.size() < Params::X!", 0);
		return false;
		//throw invalid_argument("New solution facilities is less than X!");
	}

	vector<int> facilities;
	for (Facility f : newSolution.Facilities)
	{
		facilities.push_back(f.Id);
	}
	set<int> facilitySet(facilities.begin(), facilities.end());

	for (Solution s : Population::Solutions)
	{
		bool solutionUnique = false;
		for (Facility f : s.Facilities)
		{
			if (facilitySet.find(f.Id) == facilitySet.end())
			{
				solutionUnique = true;
			}
		}
		if (!solutionUnique)
		{
			return false;
		}
	}
	return true;
}