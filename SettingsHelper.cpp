#include <vector>
#include <fstream>
#include "SettingsHelper.h"
#include "json.hpp"
#include "Params.h"
#include "InterfaceManager.h"
#include "DataHelper.h"
#include "Log.h"
#include "ProcessSynchronizer.h"
#include "RankSynchronizer.h"
#include "SolutionSynchronizer.h"
#include "PopSolutionDb.h"

using namespace std;
using json = nlohmann::json;

void SettingsHelper::ReadSettings()
{
	int rule = 0;
	try
	{
		std::ifstream settingsFile("Settings.json");
		json settingsJson;
		settingsFile >> settingsJson;

		Params::DP = settingsJson["DP"].get<int>();
		Params::CL = settingsJson["CL"].get<int>();
		Params::PF = settingsJson["PF"].get<int>();
		Params::X = settingsJson["X"].get<int>();
		Params::GA_ITERATIONS = settingsJson["GA_ITERATIONS"].get<int>();
		Params::MAX_POPULATION_SIZE = settingsJson["MAX_POPULATION_SIZE"].get<int>();
		Params::LOG = settingsJson["LOG"].get<bool>();
		Params::LOG_LEVEL = settingsJson["LOG_LEVEL"].get<int>();
		Params::ALG = settingsJson["ALG"].get<int>();
		Params::PROCESS_SYNC_ALG = settingsJson["PROCESS_SYNC_ALG"].get<int>();
		Params::AUTORUN_ENABLED = settingsJson["AUTORUN_ENABLED"].get<int>();
		Params::AUTORUN_CYCLES = settingsJson["AUTORUN_CYCLES"].get<int>();
		Params::AUTORUN_SYNC_FREQ = settingsJson["AUTORUN_SYNC_FREQ"].get<int>();
		Params::DESCRIPTION = settingsJson["DESCRIPTION"].get<string>();
		Params::SHOW_DEMAND_PROGRESS = settingsJson["SHOW_DEMAND_PROGRESS"].get<bool>();
		Params::ALG = settingsJson["ALG"].get<int>();
		rule = settingsJson["RULE"].get<int>();
		BaseAlgorithm *baseAlg = new CompleteEnumerationAlgorithm;
		ICustomerChoiceRule* choiceRule;
		ProcessSynchronizer* synchronizer;

		switch (rule)
		{
		case 0:
			choiceRule = new BinaryChoiceRule;
			break;
		case 1:
			choiceRule = new ProportionalChoiceRule;
			break;
		default:
			choiceRule = new BinaryChoiceRule;
		}

		switch (Params::ALG)
		{
		case 0:
			baseAlg = new CompleteEnumerationAlgorithm;
			break;
		case 1:
			baseAlg = new PopAlgorithm;
			break;
		case 2:
			baseAlg = new PopAlgorithmMPI;
			break;
		case 4:
			baseAlg = new PopSolutionDb;
			break;
		default:
			baseAlg = new CompleteEnumerationAlgorithm;
		}

		switch (Params::PROCESS_SYNC_ALG)
		{
		case 0:
			synchronizer = new RankSynchronizer;
			break;
		case 1:
			synchronizer = new SolutionSynchronizer;
			break;
		default:
			synchronizer = new RankSynchronizer;
		}

		InterfaceManager::InitManager(baseAlg, choiceRule, synchronizer);
	}
	catch (const std::exception &e)
	{
		Log::WriteLine("Error parsing Settings.json", 0);
        cerr << e.what() << endl;
		exit(0);
	}

	if (Params::ALG < 0 || Params::ALG > 4)
	{
		Log::WriteLine("Algorithm type invalid in settings!", 0);
		exit(0);
	}
	if (rule < 0 || rule > 1)
	{
		Log::WriteLine("Rule type invalid in settings!", 0);
		exit(0);
	}
}

void SettingsHelper::SetInputVectors(std::vector<InputPoint> &inputPoints, std::vector<DemandPoint> &dp, std::vector<Facility> &pf, std::vector<Facility> &cf)
{

	for (int i = 0; i < Params::DP; i++)
	{
		DemandPoint demandPoint;
		demandPoint.PointLocation = inputPoints[i].PointLocation;
		demandPoint.Population = inputPoints[i].Population;
		dp.push_back(demandPoint);
	}

	for (int i = 0; i < Params::PF; i++)
	{
		Facility pFacility;
		pFacility.FacilityLocation = inputPoints[i].PointLocation;
		pFacility.Quality = inputPoints[i].Quality;
		pFacility.Id = i;
		pf.push_back(pFacility);
	}

	for (int i = 0; i < Params::CL; i++)
	{
		Facility cFacility;
		cFacility.FacilityLocation = inputPoints[i].PointLocation;
		cFacility.Quality = inputPoints[i].Quality;
		cFacility.Id = i;
		cf.push_back(cFacility);
	}
}

void SettingsHelper::PrintInputPoints(std::vector<InputPoint> &inputPoints)
{
	for (InputPoint p : inputPoints)
	{
		stringstream s;
		s << p.Id << " "
		  << p.PointLocation.Longitude << " "
		  << p.PointLocation.Latitude << " "
		  << p.Population << " "
		  << p.Quality << endl;
		Log::WriteLine(s.str(), 2);
	}
}
