#include <mpi.h>
#include <sstream>
#include "GeneticAlgorithmMPI.h"
#include "ProcessSynchronizer.h"
#include "Params.h"
#include "Population.h"
#include "Log.h"
#include "InterfaceManager.h"

Solution GeneticAlgorithmMPI::GetBestSolution(int argc, char *argv[])
{
	Log::WriteLine("Genetic algorithm MPI ", 0);
	ProcessSynchronizer* synchronizer = InterfaceManager::ProcSynchronizer();
	Population p;

	InitParams(p);

	for (int i = 0; i < Params::GA_ITERATIONS; i++)
	{
		Log::WriteLine("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\   GA ITERATION " + to_string(i) + "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\'", 1);
		
		p.GenerateNewSolution();

		if (i % Params::AUTORUN_SYNC_FREQ == 0)
		{
			int bestDemand = synchronizer->Sync(p);
			if(Params::RANK == 0)
			{
				DemandProgess.push_back((double) bestDemand);
				// cout << i << "best demand " << bestDemand << endl;
			}
		}

		if (i % 20 == 0 && Params::RANK == 0)
		{
			PrintProgress((double)i / (double)Params::GA_ITERATIONS);
		}

		// Log::WriteLine("RANK " + to_string(Params::RANK) + "has reached barrier", -1);
		MPI_Barrier(MPI_COMM_WORLD);
	}
	if (Params::RANK == 0)
	{
		PrintProgress(1);
	}
	MPI_Barrier(MPI_COMM_WORLD);

	vector<int> bestSolution;
	for (Facility f : p.Solutions.at(0).Facilities)
	{
		bestSolution.push_back(f.Id);
	}

	vector<int> gatheredSolutionIds;
	gatheredSolutionIds.resize(Params::X * Params::PROCS + 1);

	MPI_Gather(bestSolution.data(), Params::X, MPI_INT, gatheredSolutionIds.data(), Params::X, MPI_INT, 0, MPI_COMM_WORLD);
	if (Params::RANK == 0)
	{
		Solution bestOverallSolution;
		for (int i = 0; i < Params::PROCS; i++)
		{
			Solution s;
			for (int j = 0; j < Params::X; j++)
			{
				int facilityId = gatheredSolutionIds[i * Params::X + j];
				Facility f = p.CandidateFacilities[facilityId];
				s.Facilities.push_back(f);
			}
			if (bestOverallSolution.CapturedDemand(p.DemandPoints, p.PreexistingFacilities) < s.CapturedDemand(p.DemandPoints, p.PreexistingFacilities))
			{
				bestOverallSolution = s;
			}
			Log::WriteLine("PROC " + to_string(i) + " BEST SOLUTION DEMAND: " + to_string(s.CapturedDemand(p.DemandPoints, p.PreexistingFacilities)), 0);
		}

		Log::WriteLine("Genetic algorithm MPI finished " + to_string(Params::RANK), 0);
		Log::WriteLine("BEST OVERALL DEMAND: " + to_string(bestOverallSolution.CapturedDemand(p.DemandPoints, p.PreexistingFacilities)), 0);
		return bestOverallSolution;
	}
	return p.Solutions.at(0);
}

void GeneticAlgorithmMPI::InitParams(Population &p)
{
	p.DemandPoints = DemandPoints;
	p.CandidateFacilities = CandidateFacilities;
	p.PreexistingFacilities = PreexistingFacilities;

	for (Facility &f : p.CandidateFacilities)
	{
		f.Rank = 1;
	}
}