#include <iostream>
#include <cmath>
#include "ResultHelper.h"

using namespace std;

double ResultHelper::AverageDemand(vector<Run> runs)
{
	double sum = 0.0;
	for (Run r : runs)
	{
		sum += r.Demand;
	}
	return RoundResult(sum / runs.size());
}

double ResultHelper::MaxDemand(vector<Run> runs)
{
	double max = 0.0;
	for (Run r : runs)
	{
		if (r.Demand > max)
		{
			max = r.Demand;
		}
	}
	return RoundResult(max);
}

double ResultHelper::StandardDeviation(vector<Run> runs)
{
	double average = AverageDemand(runs);
	double sumOfDerivation = 0.0;
	for (Run r : runs)
	{
		sumOfDerivation += r.Demand * r.Demand;
	}
	double sumOfDerivationAverage = sumOfDerivation / (runs.size() - 1);
	double res = sqrt(sumOfDerivationAverage - (average * average));
	return RoundResult(res);
}

int ResultHelper::BestRun(vector<Run> runs)
{
	double max = 0.0;
	int id = 0;
	for (Run r : runs)
	{
		if (r.Demand > max)
		{
			max = r.Demand;
			id = r.Id;
		}
	}
	return id;
}

double ResultHelper::RoundResult(double val)
{
	int r = 100.0;
	return round(val * r) / r;
}

//
//private double getStandardDeviation(List<double> doubleList)
//{
//	double average = doubleList.Average();
//	double sumOfDerivation = 0;
//	foreach(double value in doubleList)
//	{
//		sumOfDerivation += (value) * (value);
//	}
//	double sumOfDerivationAverage = sumOfDerivation / (doubleList.Count - 1);
//	return Math.Sqrt(sumOfDerivationAverage - (average * average));
//}
