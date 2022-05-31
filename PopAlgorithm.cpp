#include "stdio.h"
#include "stdlib.h"
#include "FlpAlgorithm.h"
#include "Population.h"
#include "Params.h"
#include "Log.h"
#include <math.h>

using namespace std;

Solution PopAlgorithm::GetBestSolution(int argc, char* argv[])
{
	cout << "Genetic algorithm" << endl;
	Population p;
	p.DemandPoints = DemandPoints;
	p.CandidateFacilities = CandidateFacilities;
	p.PreexistingFacilities = PreexistingFacilities;
	for (int i = 0; i < Params::GA_ITERATIONS; i++)
	{
		if (i % 100 == 0)
		{
			PrintProgress((double)i / (double)Params::GA_ITERATIONS);
		}
		
		//clock_t begin_time = clock();
		p.GenerateNewSolution();
		//std::cout << "TOTAL GENERATE" << float(clock() - begin_time) / CLOCKS_PER_SEC << endl;

		Solution s = p.Solutions.at(0);
		DemandProgess.push_back(round(s.CapturedDemand(DemandPoints, PreexistingFacilities)));
	}
	PrintProgress(1);
	Log::WriteLine("GA finished", 1);
	return p.Solutions.at(0);
}
