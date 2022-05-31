#pragma once
#include <iostream>
#include "FlpAlgorithm.h"
#include "CustomerChoiceRule.h"

using namespace std;

class Params
{
	public:
		static int DP;
		static int PF;
		static int CL;
		static int X;
		static int ALG;
		static int PROCESS_SYNC_ALG;
		static int RANK;
		static int PROCS;
		static int MAX_POPULATION_SIZE;
		static int GA_ITERATIONS;
		static bool LOG;
		static bool SHOW_DEMAND_PROGRESS;
		static int LOG_LEVEL;
		static int AUTORUN_ENABLED;
		static int AUTORUN_CYCLES;
		static int AUTORUN_SYNC_FREQ;
		static string DESCRIPTION;
};

