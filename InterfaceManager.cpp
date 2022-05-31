#include "InterfaceManager.h"

BaseAlgorithm* InterfaceManager::Algorithm;
ICustomerChoiceRule* InterfaceManager::ChoiceRule;
ProcessSynchronizer* InterfaceManager::Synchronizer;

void InterfaceManager::InitManager(BaseAlgorithm* alg, ICustomerChoiceRule* rule, ProcessSynchronizer* synchronizer)
{
	Algorithm = alg;
	ChoiceRule = rule;
	Synchronizer = synchronizer;
}

BaseAlgorithm* InterfaceManager::FlpAlgorithm()
{
	return Algorithm;
}

ICustomerChoiceRule* InterfaceManager::CustomerChoiceRule()
{
	return ChoiceRule;
}

ProcessSynchronizer* InterfaceManager::ProcSynchronizer()
{
	return Synchronizer;
}
