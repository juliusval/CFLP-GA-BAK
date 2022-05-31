#pragma once
#include "Population.h"

using namespace std;

class ProcessSynchronizer
{
public:
	virtual ~ProcessSynchronizer() {};
	virtual int Sync(Population p) = 0;
};
