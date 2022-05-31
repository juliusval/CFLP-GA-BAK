#pragma once
#include "FlpAlgorithm.h"
#include "PopAlgorithmMPI.h"
#include "CustomerChoiceRule.h"
#include "ProcessSynchronizer.h"

class InterfaceManager
{
private:
	static BaseAlgorithm* Algorithm;
	static ICustomerChoiceRule* ChoiceRule;
	static ProcessSynchronizer* Synchronizer;

public:
	static void InitManager(BaseAlgorithm*, ICustomerChoiceRule*, ProcessSynchronizer*);
	static BaseAlgorithm* FlpAlgorithm();
	static ICustomerChoiceRule* CustomerChoiceRule();
	static ProcessSynchronizer* ProcSynchronizer();
};

