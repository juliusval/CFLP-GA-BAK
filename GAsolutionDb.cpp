	#include "GAsolutionDb.h"

#include <sstream>
#include "GAsolutionDb.h"
#include "ProcessSynchronizer.h"
#include "Params.h"
#include "Population.h"
#include "Log.h"
#include "InterfaceManager.h"
#include <set>
#include "mpi.h"

int PROC_MESSAGE_SIZE = 2 + Params::X;
int MASTER_RANK_RESPONSE_SIZE = 2 + Params::CL;

// message[0] - RANK
// message[1] - STATUS
// message[2...] - BEST SOLUTION

int MASTER_PROC = 0;

enum ProcSendStatus 
{
	ProcOK = 0, 
	// Best solution sent to master
	ProcBestSolutionFound = 1, 
	// Proc finished calculations
	ProcFinished = 2,
	// Proc requesting ranks from SolutionDatabase
	ProcRequestRanks = 3,
	// Error occured for porc, need to handler it and exit program
	ProcError = -1
};


enum MasterSendStatus 
{
	MasterOK = 0, 
	// Best solution sent to master
	MasterRecieveRanks = 1,
	MasterRecieveSolutions = 2,
	MasterError = -1
};

Solution GAsolutionDb::GetBestSolution(int argc, char* argv[])
{
	PROC_MESSAGE_SIZE = 2 + Params::X;
	MASTER_RANK_RESPONSE_SIZE = 2 + Params::CL;
	if (Params::PROCS < 2)
	{
		cout << "ERROR: at least 2 PROCS needed for this algorithm to function. Rank::0 - master that gathers all solutions, other PROCS do calculations" << endl;
	}
	Population p;
	InitParams(p);
	SolutionDatabase.clear();

	// cout << "GAsolutionDb start " << Params::RANK << endl;
	if (Params::RANK == 0)
	{
		Solution s;
		s = LaunchMasterDb(p);
		// cout << "GAsolutionDb end " << Params::RANK << endl;
		return s;
	}

	int procComms = 0;
	Solution s;
	Solution bestSolutionSoFar;
	bool sendBestSolution = false;
	for (int i = 0; i < Params::GA_ITERATIONS; i++)
	{
		if (Params::RANK == 1 && i % 100 == 0)
		{
			PrintProgress((double)i / (double)Params::GA_ITERATIONS);
		}

		if(i % Params::AUTORUN_SYNC_FREQ == 0)
		{
			vector<int> rankRequestMessage;
			rankRequestMessage.resize(PROC_MESSAGE_SIZE);
			rankRequestMessage[0] = Params::RANK;
			rankRequestMessage[1] = ProcRequestRanks;
			MPI_Send(rankRequestMessage.data(), PROC_MESSAGE_SIZE, MPI_INT, MASTER_PROC, 0, MPI_COMM_WORLD);
			// cout << "Sending rank request" << endl;
			vector<int> rankResponse;
			rankResponse.resize(MASTER_RANK_RESPONSE_SIZE);

			// cout << "Ready to recieve" << endl;
			MPI_Recv(rankResponse.data(), MASTER_RANK_RESPONSE_SIZE, MPI_INT, MASTER_PROC, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			// cout << "Recieved rank response" << endl;
			switch(rankResponse[1])
			{
				case MasterOK:
					// cout << "RECIEVED OK, NOT ADJUSTING" << endl;
					break;
				case MasterRecieveRanks:
					// cout << "RECIEVED OFFSETS RANKS, ADJUSTING" << endl;
					// if(Params::RANK == 2)
					// {
					// 	cout << "RANK BEFORE ADJUSTMENT: " << endl;
					// 	for(Facility f : p.CandidateFacilities)
					// 	{
					// 		cout << f.Id << " " << f.Rank << endl;
					// 	}
					// }
					AdjustRanksFromSolutionDb(rankResponse, p);
					// if(Params::RANK == 2)
					// {
					// 	cout << "RANK AFTER ADJUSTMENT: " << endl;
					// 	for(Facility f : p.CandidateFacilities)
					// 	{
					// 		cout << f.Id << " " << f.Rank << endl;
					// 	}
					// }
					break;
			}
		}
		// cout << "Generate" << Params::RANK << endl;
		s = p.GenerateNewSolution();
		// cout << "Generate done" << Params::RANK << endl;
		if(i == 0)
		{
			bestSolutionSoFar = s;
			sendBestSolution = true;
		}

		// cout << "Searching for best solutionSoFar" << endl;
		if (bestSolutionSoFar.CapturedDemand(p.DemandPoints, PreexistingFacilities) < s.CapturedDemand(DemandPoints, PreexistingFacilities))
		{
			bestSolutionSoFar = s;
			sendBestSolution = true;
		}
		// cout << "Found best solutionSoFar" << endl;

		if(p.NewSolutionIsUnique(s))
		{
			vector<int> testMessage;
			testMessage.push_back(Params::RANK);
			ProcSendStatus status = ProcBestSolutionFound;
			testMessage.push_back(status);
			for (Facility f : s.Facilities) 
			{
				testMessage.push_back(f.Id);
			}

			// cout << "MPI_Send (i: " << i << ")" << Params::RANK << endl;
			MPI_Send(testMessage.data(), PROC_MESSAGE_SIZE, MPI_INT, MASTER_PROC, 0, MPI_COMM_WORLD);
			// cout << "MPI_Send DONE" << Params::RANK << endl;
			procComms++;
		}
		sendBestSolution = false;
	}

	vector<int> finishMessage;
	finishMessage.push_back(Params::RANK);
	ProcSendStatus statusFin = ProcFinished;
	finishMessage.push_back(statusFin);
	for (Facility f : p.Solutions.at(0).Facilities) 
	{
		finishMessage.push_back(-1);
	}
	// cout << "Sending finish message" << endl;
	MPI_Send(finishMessage.data(), PROC_MESSAGE_SIZE, MPI_INT, MASTER_PROC, 0, MPI_COMM_WORLD);
	// cout << "Finish message sent" << Params::RANK << endl;
	cout << "PROC " << Params::RANK << "Comms count: " << procComms << endl;
	return s;
}

void GAsolutionDb::AdjustRanksFromSolutionDb(vector<int> rankOffsetsMsg, Population& p)
{
	vector<int> rankOffsets;
	rankOffsets = vector<int>(rankOffsetsMsg.begin() + 2, rankOffsetsMsg.end());
	for (int j = 0; j < Params::CL; j++)
	{
		// if(Params::RANK == 2 && rankOffsets[j] != 0)
		// {
		// 	cout << "ID: " << j << "Rank: " << p.CandidateFacilities[j].Rank << endl;
		// }
		if (p.CandidateFacilities[j].Rank > 1) 
		{
			int offset = rankOffsets[j];
			if(offset > 50)
				offset = 50;
			if(offset < -50)
				offset = -50;

			double adjustment = ((double)offset / 100.0);
			int offsetRank = (int)(adjustment * (double)p.CandidateFacilities[j].Rank);
			if(offsetRank == 0)
			{
				if(adjustment < 0)
					offsetRank = -1; 
					
				if(adjustment > 0)
					offsetRank = 1; 
			}
			if(p.CandidateFacilities[j].Rank > 100 && offsetRank > 5)
			{
				offsetRank = 5;
			}
			//Reset rank if adjustment would be beyond 0
			// if(Params::RANK == 2 && rankOffsets[j] != 0)
			// {
			// 	cout << "Adjusting ID " << j << " Rank before: " << p.CandidateFacilities[j].Rank << " adjustment: " << adjustment << " offsetRank: " << offsetRank << endl;
			// }
			p.CandidateFacilities[j].Rank += offsetRank;
		} 
		// cout << "Recieved rank offset " << rankOffsets[j] << endl;
		// Adjusts rank depending on how many S TIER or F TIER solutions it is in, 
		// if (rankOffsets[j] < 0 && p.CandidateFacilities[j].Rank <= abs(rankOffsets[j])) 
		// {
		// 	//Reset rank if adjustment would be beyond 0
		// 	p.CandidateFacilities[j].Rank = 1;
		// } 
		// else
		// {
		// 	// cout << "adjusting id " << j << " with rank " << p.CandidateFacilities[j].Rank << " by offset " << rankOffsets[j] << endl; 
		// 	p.CandidateFacilities[j].Rank += rankOffsets[j];
		// 	// cout << "adjusted rank: " << p.CandidateFacilities[j].Rank << endl; 
		// }
	}
}

Solution GAsolutionDb::LaunchMasterDb(Population p)
{
	cout << "Launch master db" << endl;
	vector<int> solutionSet;
	vector<int> dbRankOffsets;
	vector<Solution> solutionDatabase;
	int procsFinished = 0;
	int CommsCount = 0;
	ProcSendStatus status;
	int procRank = 0;
	while (procsFinished != Params::PROCS - 1)
	{
		vector<int> ProcData;
		ProcData.resize(PROC_MESSAGE_SIZE + 1);
		// cout << "MPI_Recv" << Params::RANK << endl;
		MPI_Recv(ProcData.data(), PROC_MESSAGE_SIZE, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		procRank = ProcData[0];
		status = static_cast<ProcSendStatus>(ProcData[1]);
		// cout << "MPI_Recv done " << Params::RANK << " recieved from " << ProcData[0] << endl;
		// for(int data : ProcData)
		// {
		// 	cout << data << " ";
		// }
		// cout << "Recieved proc data printed" << endl;
		if (ProcData.empty() || ProcData.size() < PROC_MESSAGE_SIZE)
		{
			cout << "ERROR PARSING PROC MESSAGE, EXITING PROGRAM" << endl;
			exit(0);
		}

		try
		{
			switch (status)
			{
			case ProcOK:
				// cout << "PROC " << procRank << " STATUS OK" << endl;
				break;
			case ProcBestSolutionFound:
				// cout << "Best solution found from proc " << procRank << endl;
				HandleBestSolutionFound(p, ProcData);
				CommsCount++;
				break;
			case ProcRequestRanks:
				// cout << "Rank request recieved master" << endl;
				dbRankOffsets.clear();
				dbRankOffsets = SolutionDatabaseRanksWithAdjustment();
				// cout << "Rank offsets build" << endl;
				if(!SolutionDatabase.empty() && SolutionDatabase.size() > 100)
				{
					// cout << "SEND RANK OFFSETS" << endl;
					// cout << "dbRankOffsets SIZE: " << to_string(dbRankOffsets.size()) << " MASTER_RANK_RESPONSE_SIZE " << MASTER_RANK_RESPONSE_SIZE << endl;
					MPI_Send(dbRankOffsets.data(), MASTER_RANK_RESPONSE_SIZE, MPI_INT, procRank, 0, MPI_COMM_WORLD);
					
					//PrintSolutionDatabase(p);
					// cout << "Sent requested ranks to proc " << procRank << endl;
				}
				else
				{
					// cout << "SEND OK" << endl;
					// cout << "dbRankOffsets SIZE: " << to_string(dbRankOffsets.size()) << " MASTER_RANK_RESPONSE_SIZE " << MASTER_RANK_RESPONSE_SIZE << endl;
					MPI_Send(dbRankOffsets.data(), MASTER_RANK_RESPONSE_SIZE, MPI_INT, procRank, 0, MPI_COMM_WORLD);
					// cout << "Solution database not big enough, send OK " << procRank << endl;
				}
				// log best solution progress if rank sync engaged
				if (procRank == 1)
				{
					Solution s;
					s = SolutionDatabase.at(0);
					int bestDemand = (int) s.Demand;
					if (Params::RANK == 0)
					{
						DemandProgess.push_back(bestDemand);
						// cout << "best demand " << bestDemand << endl;
					}
				}
				break;
			case ProcFinished:
				procsFinished++;
				// cout << "PROC FINISHED!!! " << procRank << endl;
				break;
			case ProcError:
				cout << "Error occured on PROC with rank " << procRank << endl;
				break;
			}
		}
		catch (const std::exception& exc)
		{
			cout << "ERROR IN MASTER DB, message from  " <<  procRank << "status: " << status << endl;
			cerr << exc.what() << endl;
		}
	}
	cout << "FINISHED MASTER DB, comms count: " << CommsCount << endl;
	Solution fakeSolution;
	return SolutionDatabase.empty() ? fakeSolution : SolutionDatabase.at(0);
}

vector<int> GAsolutionDb::SolutionDatabaseRanks()
{
	for (Facility& fc : CandidateFacilities)
	{
		fc.Rank = 0;
	}
	for (Solution &s : SolutionDatabase)
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
	vector<int> ranks;
	for (Facility& fc : CandidateFacilities)
	{
		ranks.push_back(fc.Rank);
	}
	return ranks;
}


vector<int> GAsolutionDb::SolutionDatabaseRanksWithAdjustment()
{
	double topDemand = 0;
	if(SolutionDatabase.empty() || SolutionDatabase.size() <= 100)
	{
		// cout << "RETURNING MASTEROK" << endl;
		vector<int> rankOffsets;
		rankOffsets.resize(MASTER_RANK_RESPONSE_SIZE);
		rankOffsets[0] = 0;
		rankOffsets[1] = MasterOK;
		return rankOffsets;
	}
	topDemand = SolutionDatabase.at(0).Demand;
	// cout << "top demand: " << topDemand << endl;
	// cout << "Building ranks further" << endl;
	int sTierBonus = 5;
	int fTierPenalty = 5;
	double sTier = topDemand * 0.7;
	double fTier = topDemand * 0.99;
	int fPenalized = 0;
	int fBonus = 0;

	for (Facility& fc : CandidateFacilities)
	{
		fc.Rank = 0;
	}

	int howMuchOffTheTop = (int) (((double)SolutionDatabase.size()) * 0.1);
	
	int howMuchfromTheBottom = (int) (((double)SolutionDatabase.size()) * 0.9);

	// cout << "db size: " << SolutionDatabase.size() << " off the top: " << howMuchOffTheTop  << "fromTheBottom: " << howMuchfromTheBottom << endl;
	for(int i = 0; i < howMuchOffTheTop; i++)
	{
		Solution s = SolutionDatabase[i];
		for (Facility& f : s.Facilities)
		{
			CandidateFacilities[f.Id].Rank += sTierBonus;
		}
	}

	for(int i = howMuchfromTheBottom + 1; i < SolutionDatabase.size(); i++)
	{
		Solution s = SolutionDatabase[i];
		for (Facility& f : s.Facilities)
		{
			CandidateFacilities[f.Id].Rank -= fTierPenalty;
		}
	}
	vector<int> rankOffsets;
	rankOffsets.push_back(MASTER_PROC);
	rankOffsets.push_back(MasterRecieveRanks);
	for(int i = 0; i < Params::CL; i++)
	{
		rankOffsets.push_back(CandidateFacilities[i].Rank);
		// cout << "Rank adjustment for ID " << i << " " << CandidateFacilities[i].Rank << endl;
	}
	// cout << "penalized: " << fPenalized << endl;
	// cout << "bonused: " << fBonus << endl;
	// cout << "RANKS BUILT!" << endl;
	return rankOffsets;
}

void GAsolutionDb::HandleBestSolutionFound(Population& p, vector<int> procData)
{
	// cout << "HandleBestSolutionFound from " << procData[0] << endl;
	Solution s;
	vector<int> solutionIds;
	solutionIds = vector<int>(procData.begin() + 2, procData.end());
	for (int j = 0; j < Params::X; j++)
	{
		int facilityId = solutionIds[j];
		Facility f = p.CandidateFacilities[facilityId];
		s.Facilities.push_back(f);
	}

	if( SolutionSet.find(solutionIds) != SolutionSet.end())
	{
		// cout << "HandleBestSolutionFound early end: solution already in database" << endl;
		return;
	}
	SolutionSet.insert(solutionIds);

	s.CapturedDemand(p.DemandPoints, p.PreexistingFacilities);
	if(SolutionDatabase.empty())
	{
		SolutionDatabase.push_back(s);
	}
	else
	{
		for (int i = 0; i < SolutionDatabase.size(); i++)
		{
			if (i == 0)
			{	 
				if (SolutionDatabase.empty())
				{
					SolutionDatabase.push_back(s);
					break;
				} 
				else if (SolutionDatabase[0].Demand <= s.Demand)
				{
					SolutionDatabase.insert(SolutionDatabase.begin(), s);
					break;
				}
				else if (SolutionDatabase[0].Demand >= s.Demand && SolutionDatabase[i+1].Demand <= s.Demand)
				{
					SolutionDatabase.insert(SolutionDatabase.begin()+1, s);
					break;
				}
			}
			else if(i == SolutionDatabase.size() - 1)
			{
				if(SolutionDatabase[i].Demand > s.Demand)
				{
					SolutionDatabase.push_back(s);
					break;
				}
				else
				{
					SolutionDatabase.insert(SolutionDatabase.begin() + i, s);
					break;
				}
			}
			else
			{
				if (SolutionDatabase[i].Demand >= s.Demand && SolutionDatabase[i+1].Demand <= s.Demand)
				{
					SolutionDatabase.insert(SolutionDatabase.begin() + (i + 1), s);
					break;
				}
			}
		}
	}

	// cout << "HandleBestSolutionFound end " << procData[0] << endl;
}

void GAsolutionDb::PrintSolutionDatabase(Population p)
{
	cout << "_________SOLUTION___DATABASE__________" << endl;
	for (Solution s : SolutionDatabase)
	{
		cout << s.ToString();
	}
	cout << "______________________________________" << endl;
	
	if(!SolutionDatabase.empty() && SolutionDatabase.size() > 3)
	{
		for(int i = 0; i < SolutionDatabase.size(); i++)
		{
			if(i < SolutionDatabase.size() - 2 && SolutionDatabase[i].Demand < SolutionDatabase[i+1].Demand)
			{
				cout << "ERROR: Solution database wrong insertion" << endl;
				MPI_Barrier(MPI_COMM_WORLD);
				exit(0);
			}
		}
	}
}

void GAsolutionDb::InitParams(Population& p)
{
	p.DemandPoints = DemandPoints;
	p.CandidateFacilities = CandidateFacilities;
	p.PreexistingFacilities = PreexistingFacilities;
	

	for (Facility& f : p.CandidateFacilities)
	{
		f.Rank = 1;
	}
}