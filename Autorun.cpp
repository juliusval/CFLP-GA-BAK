#include <fstream>
#include <ctime>
#include "Autorun.h"
#include "json.hpp"
#include "DataHelper.h"
#include "DataHelper.h"
#include "Facility.h"
#include "DemandPoint.h"
#include "InterfaceManager.h"
#include "SettingsHelper.h"
#include "Params.h"
#include "Log.h"
#include "Run.h"
#include "ResultHelper.h"

using namespace std;
using json = nlohmann::json;

void Autorun::Start(int argc, char* argv[])
{
	Log::WriteLine("AUTORUN START", 0);

	nlohmann::ordered_json resultsJson;
	vector<DemandPoint> dp;
	vector<Facility> pf, cf;
	vector<Run> runs;
	int failedRuns = 0;
	time_t currentTime = time(NULL);
	string timeStr = ctime(&currentTime);
	resultsJson["AUTORUN_CYCLES"] = Params::AUTORUN_CYCLES;
	resultsJson["PROCS"] = Params::PROCS;
	resultsJson["AUTORUN_SYNC_FREQ"] = Params::AUTORUN_SYNC_FREQ;
	resultsJson["ALG"] = Params::ALG;
	resultsJson["PROCESS_SYNC_ALG"] = Params::PROCESS_SYNC_ALG;
	resultsJson["GA_ITERATIONS"] = Params::GA_ITERATIONS;
	resultsJson["DP"] = Params::DP;
	resultsJson["CL"] = Params::CL;
	resultsJson["PF"] = Params::PF;
	resultsJson["X"] = Params::X;
	resultsJson["DESCRIPTION"] = Params::DESCRIPTION;
	resultsJson["TIMESTAMP_START"] = timeStr;

	vector<InputPoint> inputPoints = DataHelperFile::FetchPoints();
	SettingsHelper::SetInputVectors(inputPoints, dp, pf, cf);

	for (Facility &f : cf)
	{
		f.Rank = 1;
	}

	BaseAlgorithm* flpAlgorithm = InterfaceManager::FlpAlgorithm();
	flpAlgorithm->Init(dp, pf, cf);

	json runResults = json::array();
	Log::WriteLine("AUTORUN PREP DONE", 0);
	for (int i = 0; i < Params::AUTORUN_CYCLES; i++)
	{
		stringstream autorunState;
		autorunState << "CYCLE " << i << " OUT OF " << Params::AUTORUN_CYCLES << endl;
		Log::WriteLine(autorunState.str(), 0);
		try
		{
			
			const clock_t begin_time = clock();
			Solution solution = flpAlgorithm->GetBestSolution(argc, argv);
			std::cout << "GetBestSolution time: " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << endl;

			Log::WriteLine(solution.ToString(), 0);
			json cycleResult;
			cycleResult["Id"] = i;
			cycleResult["Demand"] = (int) round(solution.Demand);
			json solutionArr = json::array();
			for (Facility f : solution.Facilities)
			{
				solutionArr.insert(solutionArr.end(), f.Id);
			}
			cycleResult["Solution"] = solutionArr;
			cycleResult["_DemandProgress"] = {};
			if (Params::SHOW_DEMAND_PROGRESS)
			{
				cycleResult["_DemandProgress"] = flpAlgorithm->DemandProgess;
			}
			runResults.insert(runResults.end(), cycleResult);
			Run run;
			run.Id = i;
			run.Demand = solution.Demand;
			run.solution = solution;
			runs.push_back(run);
			flpAlgorithm->DemandProgess.clear();
		}
		catch (const std::exception& e)
		{
			json errorResult;
			errorResult["Id"] = i;
			errorResult["Error"] = e.what();
			Log::WriteLine("Error occured in run #" + to_string(i) + " RANK: " + to_string(Params::RANK), 0);
			Log::WriteLine(e.what(), 0);
			runResults.insert(runResults.end(), errorResult);
			failedRuns++;
		}
	}

	if (Params::RANK == 0)
	{
		try
		{
			currentTime = time(NULL);
			timeStr = ctime(&currentTime);
			resultsJson["TIMESTAMP_END"] = timeStr;
			resultsJson["FAILED_RUNS"] = failedRuns;
			resultsJson["BEST_RUN"] = ResultHelper::BestRun(runs);
			resultsJson["AVERAGE_DEMAND"] = ResultHelper::AverageDemand(runs);
			cout << "AVG: " << ResultHelper::AverageDemand(runs) << endl;
			resultsJson["MAX_DEMAND"] = ResultHelper::MaxDemand(runs);
			resultsJson["STAND_DEV"] = ResultHelper::StandardDeviation(runs);
			resultsJson["RUNS"] = runResults;
		}
		catch (const std::exception&)
		{
			Log::WriteLine("Error writing result json!", 0);
		}
		ofstream file("Result.json");
		stringstream logFileName;
		logFileName << "./RunResults/Result_" << currentTime << ".json";
		ofstream fileLog(logFileName.str());
		file << setw(4) << resultsJson << endl;
		fileLog << setw(4) << resultsJson << endl;
		file.close();
		fileLog.close();
		Log::WriteLine("AUTORUN END", 0);
		Log::Close();
	}
}