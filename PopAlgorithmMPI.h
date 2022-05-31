#pragma once

#include <iostream>
#include <vector>
#include "FlpAlgorithm.h"
#include "Solution.h"
#include "DemandPoint.h"
#include "Facility.h"
#include "Population.h"

using namespace std;

class PopAlgorithmMPI : public BaseAlgorithm {
public:
	virtual Solution GetBestSolution(int argc, char* argv[]);
	void InitParams(Population& p);
};