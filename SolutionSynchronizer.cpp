#include <iostream>
#include <sstream>
#include <vector>
#include "RankSynchronizer.h"
#include "Log.h"
#include "Params.h"
#include "mpi.h"
#include "SolutionSynchronizer.h"
#include <algorithm>
#include <stdlib.h>

using namespace std;

int SolutionSynchronizer::Sync(Population p)
{
	vector<int> solutions, partialGrid;
	std::vector<Solution> finalSolutions;
	solutions.resize(Params::X * (Params::PROCS)+1);

	 cout << "---------S SYNC-------------" << endl;
	//PrintSolutions(p);

	for (Facility f : p.Solutions.at(0).Facilities)
	{
		partialGrid.push_back(f.Id);
	}

	MPI_Gather(partialGrid.data(), Params::X, MPI_INT, solutions.data(), Params::X, MPI_INT, 0, MPI_COMM_WORLD);
	// cout << "GATHER SUCCESS" << endl;
	try
	{
		finalSolutions = BuildFinalSolutions(p, solutions);
	}
	catch (const std::exception &exc)
	{
		cout << "ERROR ON PROC " << Params::RANK << endl;
		std::cerr << exc.what();
	}		
	// cout << "BUILD FINAL SOLUTIONS SUCCESS " << Params::RANK << endl;

	vector<int> finalSolutionIds;
	for (Solution s : finalSolutions)
	{
		for (Facility f : s.Facilities)
		{
			finalSolutionIds.push_back(f.Id);
		}
	}
	MPI_Barrier(MPI_COMM_WORLD);
	// cout << "BCAST PREP" << Params::RANK << endl;

	MPI_Bcast(finalSolutionIds.data(), finalSolutionIds.size(), MPI_INT, 0, MPI_COMM_WORLD);

	// cout << "BCAST SUCCESS " << Params::RANK << endl;
	MPI_Barrier(MPI_COMM_WORLD);

	vector<Solution> broadcastedSolutions;
	vector<Facility> tempFacilities;
	for (int i = 0; i < finalSolutionIds.size(); i++)
	{
		Facility f;
		// cout << "FETCHING BROADCASTED SOLUTIONS " << i << endl;
		f = p.CandidateFacilities[finalSolutionIds[i]];
		tempFacilities.push_back(f);
		if (tempFacilities.size() == Params::X)
		{
			Solution s;
			s.Facilities = tempFacilities;
			broadcastedSolutions.push_back(s);
			tempFacilities.clear();
		}
	}


	for (int i = 0; i < broadcastedSolutions.size() && i < p.Solutions.size(); i++)
	{
		// cout << i << "Comparing demands" << endl;
		if (broadcastedSolutions[i].CapturedDemand(p.DemandPoints, p.PreexistingFacilities) > p.Solutions[i].CapturedDemand(p.DemandPoints, p.PreexistingFacilities))
		{
			// cout << i << "Demand is better" << endl;
			// cout << broadcastedSolutions[i].Demand << "broadcastedSolutions[i]" << endl;
			// cout << p.Solutions[i].Demand  << "p.solutions[i]" << endl;
			p.Solutions[i] = broadcastedSolutions[i];
			// cout << i << "After set!" << endl;
		}
		else
		{
			// cout << i << "Demand is worse " << endl;
		}
	}
	// cout << "SET CAPTURED DEMAND SUCCESS " << Params::RANK << endl;
	// PrintSolutions(p);
	// cout << "||||||||||||||| S SYNC OVER |||||||||||" << endl;
	return 0;
}

void SolutionSynchronizer::PrintSolutions(Population& p)
{
	for (int i = 0; i < Params::PROCS; i++)
	{
		if (i == Params::RANK)
		{
			cout << "RANK: " << i << endl;
			for (Solution s : p.Solutions)
			{
				cout << "Solution facilities: ";
				for (Facility f : s.Facilities)
				{
					cout << f.Id << ", ";
				}
				cout << ". Demand: " << to_string(s.CapturedDemand(p.DemandPoints, p.PreexistingFacilities)) << endl;
			}
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}
}

vector<Solution> SolutionSynchronizer::BuildFinalSolutions(Population p, vector<int> solutions)
{
	if (Params::RANK != 0)
	{
		return vector<Solution>();
	}
	vector<int> allSolutionsIds;
	vector<Solution> allSolutions;
	vector<Solution> bestSolutions;
	vector<double> demands;
	// if(Params::RANK == 0)
	// {
	// 	cout << "break1" << endl;
	// }
	for (int i = 0; i < Params::PROCS; i++)
	{
		Solution s;
		for (int j = 0; j < Params::X; j++)
		{
			Facility f;
			int facilityId = solutions[i * Params::X + j];
			f = p.CandidateFacilities[facilityId];
			f.Id = facilityId;
			s.Facilities.push_back(f);
		}
		demands.push_back(s.CapturedDemand(p.DemandPoints, p.PreexistingFacilities));
		allSolutions.push_back(s);
	}
	// if(Params::RANK == 0)
	// {
	// 	cout << "break2" << endl;
	// }

	sort(demands.begin(), demands.end(), sort_using_greater_than);
	for (int d = 1; d < Params::PROCS; d++)
	{
		// if(Params::RANK == 0)
		// {
		// 	cout << " demands.size(): " << demands.size() << endl;
		// 	cout << "d: " << d << endl;
		// 	cout << "demands[d]: " << demands[d] << endl;
		// 	cout << " demands[d - 1]: " << demands[d-1] << endl;
		// }
		if (demands.size() > 1 && d < demands.size() && d >= 1 && abs(demands[d] - demands[d - 1]) < 2)
		{
			// if(Params::RANK == 0)
			// {
			// 	cout << " demands.erase() " << demands[d-1] << endl;
			// }
			demands.erase(demands.begin() + d);
			// if(Params::RANK == 0)
			// {
			// 	cout << " demands.erase() SUCCESS" << demands[d-1] << endl;
			// }
			d--;
		}
	}
	// if(Params::RANK == 0)
	// {
	// 	cout << "break3" << endl;
	// }
	//Max best solutions should not be more than MAX_POPULATION_SIZE
	int demandsCutOff = Params::MAX_POPULATION_SIZE - 1;
	if(demands.size() > demandsCutOff)
		demands = vector<double>(demands.begin(), demands.begin() + Params::MAX_POPULATION_SIZE - 1);

	//Best demands are collected, now check which one is the last good one
	int lastGoodDemandPosition = Params::MAX_POPULATION_SIZE - 1;
	if (demands.size() < Params::MAX_POPULATION_SIZE)
	{
		lastGoodDemandPosition = demands.size() - 1;
	}
	// if(Params::RANK == 0)
	// {
	// 	cout << "break4" << endl;
	// }

	double demandToMatch = demands[lastGoodDemandPosition];

	vector<Solution> filteredBestSolutions;
	for (Solution s : allSolutions)
	{
		if (s.Demand >= demandToMatch)
		{
			filteredBestSolutions.push_back(s);
		}
	}
	// if(Params::RANK == 0)
	// {
	// 	cout << "break5" << endl;
	// }

	return filteredBestSolutions;
}

bool SolutionSynchronizer::sort_using_greater_than(double u, double v)
{
	return u > v;
}