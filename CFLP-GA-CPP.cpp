#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include "Location.h"
#include "DataHelper.h"
#include "InterfaceManager.h"
#include "CFLP-GA-CPP.h"
#include "FlpAlgorithm.h"
#include "PopAlgorithmMPI.h"
#include "Solution.h"
#include "Params.h"
#include "json.hpp"
#include <sstream>
#include "Log.h"
#include "SettingsHelper.h"
#include "Autorun.h"
#include "mpi.h"

using namespace std;
using json = nlohmann::json;


int main(int argc, char *argv[])
{
	int rank, numOfProcs;
	
	Log::WriteLine("GENETIC MPI !");
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numOfProcs);
	cout << "MPI INIT " + to_string(rank) << endl;
	Params::RANK = rank;
	Params::PROCS = numOfProcs;
	srand ( time(NULL) + Params::RANK);
	
	SettingsHelper::ReadSettings();
	Log::Open("Logs" + to_string(Params::RANK) + ".txt");
	if(Params::RANK == 0)
	{
		Log::WriteLine("Description: " + Params::DESCRIPTION, 0);
	}

	Log::WriteLine("PROCS: " + to_string(Params::PROCS), 0);
	MPI_Barrier(MPI_COMM_WORLD);

	if (Params::AUTORUN_ENABLED == 1)
	{
		Autorun::Start(argc, argv);
		Log::WriteLine("End of MPI program", 1);
		MPI_Barrier(MPI_COMM_WORLD);
		cout << "FINALIZE " + to_string(rank) << endl;
		MPI_Finalize();
		exit(0);
	}

	vector<DemandPoint> dp;
	vector<Facility> pf, cf;
	Solution solution;


	vector<InputPoint> inputPoints = DataHelperFile::FetchPoints();
	SettingsHelper::SetInputVectors(inputPoints, dp, pf, cf);

	BaseAlgorithm* flpAlgorithm = InterfaceManager::FlpAlgorithm();
	flpAlgorithm->Init(dp, pf, cf);

	solution = flpAlgorithm->GetBestSolution(argc, argv);

	Log::WriteLine(solution.ToString(), 0);
	Log::WriteLine("End of program", 1);
	MPI_Finalize();
	Log::Close();
	exit(0);
}
