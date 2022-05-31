#include <sstream>
#include "RankSynchronizer.h"
#include "Log.h"
#include "Params.h"
#include "mpi.h"
int debug_sync = 1;

int RankSynchronizer::Sync(Population p)
{
	int bestDemand = -1;
	if (debug_sync == 1 && Params::RANK == 0)
	{
		cout << "rank sync" << endl;
		debug_sync = 0;
	}

	std::vector<int> ranks, finalRanks, partialGrid;
	ranks.resize(Params::CL * (Params::PROCS)+1);
	finalRanks.resize(Params::CL + 1);
	partialGrid.resize(Params::CL + 1);
	fill(finalRanks.begin(), finalRanks.end(), 0);

	PrepareRankSharing(partialGrid, p);

	MPI_Gather(partialGrid.data(), Params::CL, MPI_INT, ranks.data(), Params::CL, MPI_INT, 0, MPI_COMM_WORLD);

	LogRankSharing(ranks, finalRanks);

	Solution s;
	s = p.Solutions.at(0);
	int demand = s.Demand;
	vector<int> bestDemands;
	bestDemands.resize(Params::PROCS);

	MPI_Gather(&demand, 1, MPI_INT, bestDemands.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);
	if (Params::RANK == 0)
	{
		for (int d : bestDemands)
		{
			if (bestDemand < d)
				bestDemand = d;
		}
	}

	MPI_Bcast(finalRanks.data(), Params::CL, MPI_INT, 0, MPI_COMM_WORLD);

	AssignSharedRanks(finalRanks, p.CandidateFacilities);

	MPI_Barrier(MPI_COMM_WORLD);

	if (Params::LOG_LEVEL > 0)
	{
		for (int j = 0; j <= Params::PROCS; j++)
		{
			if (Params::RANK == j)
			{
				LogProcRanks(p.CandidateFacilities);
			}
			MPI_Barrier(MPI_COMM_WORLD);
		}
	}

	return bestDemand;
}

void RankSynchronizer::PrepareRankSharing(std::vector<int>& partialGrid, Population& p)
{
	Log::WriteLine("Sharing ranking data! " + to_string(Params::RANK), 1);
	partialGrid.clear();
	for (Facility& f : p.CandidateFacilities)
	{
		partialGrid.push_back(f.Rank);
		stringstream s;
		s << Params::RANK << " PROC facility with ID " << f.Id << " Rank " << f.Rank;
		Log::WriteLine(s.str(), 1);
	}
}

void RankSynchronizer::AssignSharedRanks(vector<int>& finalRanks, vector<Facility>& candidateFacilities)
{
	for (int i = 0; i < Params::CL; i++)
	{
		candidateFacilities[i].Rank = finalRanks[i];
	}
}

void RankSynchronizer::LogRankSharing(std::vector<int>& ranks, std::vector<int>& finalRanks)
{
	if (Params::RANK == 0)
	{
		Log::WriteLine("Gather done ", 1);
		Log::WriteLine("PRINTING PROC DATA", 1);
		for (int i = 0; i < Params::PROCS; i++)
		{
			Log::WriteLine("PROC " + to_string(i) + " RANKS", 1);
			for (int j = 0; j < Params::CL; j++)
			{
				Log::WriteLine("Candidate " + to_string(j) + " Rank: " + to_string(ranks[(i * Params::CL) + j]), 1);
				finalRanks[j] += ranks[(i * Params::CL) + j];
			}
			Log::WriteLine("------------------------------------------------", 1);
		}

		int minRank = 99999999;
		for (int j = 0; j < Params::CL; j++)
		{
			if (minRank > finalRanks[j])
			{
				minRank = finalRanks[j];
			}
		}
		for (int j = 0; j < Params::CL; j++)
		{
			finalRanks[j] -= (minRank - 1);
		}

		LogFinalRanks(ranks, finalRanks);
	}
}

void RankSynchronizer::LogProcRanks(std::vector<Facility>& candidateFacilities)
{
	Log::WriteLine("PROC " + to_string(Params::RANK) + " RANKS", 1);
	for (int j = 0; j < Params::CL; j++)
	{
		Log::WriteLine("Candidate " + to_string(candidateFacilities[j].Id) + " Rank: " + to_string(candidateFacilities[j].Rank), 1);
	}
}

void RankSynchronizer::LogFinalRanks(std::vector<int>& ranks, std::vector<int>& finalRanks)
{
	if (Params::RANK == 0)
	{
		Log::WriteLine("=========================", 1);
		for (int r : finalRanks)
		{
			Log::WriteLine("Final rank: " + to_string(r), 1);
		}
		Log::WriteLine("=========================", 1);
	}
}
