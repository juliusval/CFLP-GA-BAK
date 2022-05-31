
//
//class Params;
//
//class Location
//{
//public:
//	double Longitude;
//	double Latitude;
//};
//
//
//class Facility
//{
//public:
//	int Id;
//	int Quality;
//	Location FacilityLocation;
//};
//
//class LocationHelper
//{
//public:
//	static double DistanceToLocation(Location fromLocation, Location toLocation)
//	{
//		double dlon = fabs(fromLocation.Longitude - toLocation.Longitude);
//		double dlat = fabs(fromLocation.Latitude - toLocation.Latitude);
//		double aa = pow((sin((double)dlon / (double)2 * 0.01745)), 2) +
//			cos(fromLocation.Longitude * 0.01745) *
//			cos(toLocation.Longitude * 0.01745) *
//			pow((sin((double)dlat / (double)2 * 0.01745)), 2);
//		double c = 2 * atan2(sqrt(aa), sqrt(1 - aa));
//		return 6371 * c;
//	}
//};
//
//class DemandPoint
//{
//public:
//	int Population;
//	Location PointLocation;
//	double DistanceToPoint(Location fromLocation)
//	{
//		return LocationHelper::DistanceToLocation(fromLocation, PointLocation);
//	}
//};
//
//class ICustomerChoiceRule
//{
//public:
//	virtual ~ICustomerChoiceRule() {}
//	virtual double CapturedDemand(vector<DemandPoint> demandPoints,
//		vector<Facility> preexistingFacilities,
//		vector<Facility> newFacilities) = 0;
//
//};
//
//class BinaryChoiceRule : ICustomerChoiceRule
//{
//public:
//	virtual double CapturedDemand(vector<DemandPoint> demandPoints,
//		vector<Facility> preexistingFacilities,
//		vector<Facility> newFacilities)
//	{
//		return 0;
//	}
//};
//
//class ProportionalChoiceRule
//{
//public:
//	virtual double CapturedDemand(vector<DemandPoint> demandPoints,
//		vector<Facility> preexistingFacilities,
//		vector<Facility> newFacilities)
//	{
//		return 0;
//	}
//};
//
//class Solution
//{
//public:
//	vector<Facility> Facilities;
//	double Demand = -1;
//	double CapturedDemand(vector<DemandPoint> demandPoints, vector<Facility> preexistingFacilities)
//	{
//		if (Demand == -1)
//		{
//			Demand = Params::CustomerChoiceRule->CapturedDemand(demandPoints, preexistingFacilities, Facilities);
//
//			return Demand;
//		}
//
//	}
//
//	string ToString()
//	{
//		stringstream ss;
//		for (Facility f : Facilities)
//		{
//			ss << " " + f.Id;
//		}
//		ss << "Facilities: " << ss.str();
//		return ss.str();
//	}
//}
//
//class BaseAlgorithm
//{
//public:
//	virtual ~BaseAlgorithm() {}
//
//	vector<DemandPoint> DemandPoints;
//	vector<Facility> PreexistingFacilities;
//	vector<Facility> CandidateFacilities;
//
//	virtual Solution GetBestSolution() = 0;
//};
//
//class Params
//{
//public:
//	const int DP = 1000;
//	const int PF = 10;
//	const int CL = 25;
//	const int X = 3;
//
//	static BaseAlgorithm* FlpAlgorithm;
//	static ICustomerChoiceRule* CustomerChoiceRule;
//};
//
//class CompleteEnumerationAlgorithm : BaseAlgorithm {
//public:
//	Solution GetBestSolution()
//	{
//		Solution* s;
//		return *s;
//	}
//};
//
//BaseAlgorithm* Params::FlpAlgorithm;
//ICustomerChoiceRule* Params::CustomerChoiceRule;
//
//
//class InputPoint
//{
//public:
//	int Id;
//	Location PointLocation;
//	int Population;
//	int Quality;
//};
//
//
//class Population
//{
//public:
//	vector<DemandPoint> DemandPoints;
//	vector<Facility> PreexistingFacilities;
//	vector<Facility> CandidateFacilities;
//
//	double GetSolutionSamplingProbability(Solution solution)
//	{
//		return 0;
//	}
//
//	Solution GenerateNewSolution()
//	{
//		Solution* s = NULL;
//		return *s;
//	}
//};
//
//class IPointDataHelper
//{
//public:
//	virtual vector<InputPoint> FetchPoints() = 0;
//};
//
//class DataHelperFile
//{
//public:
//	virtual vector<InputPoint> FetchPoints()
//	{
//		vector<InputPoint> p;
//		FILE* f;
//		f = fopen("demandPointsRandomQuality.dat", "r");
//		for (int i = 0; i < DP; i++)
//		{
//			double* demandPoint = new double[4];
//			fscanf(f, "%lf%lf%lf%lf", &demandPoint[0], &demandPoint[1], &demandPoint[2], &demandPoint[3]);
//			InputPoint inputPoint;
//			Location location;
//			location.Longitude = demandPoint[0];
//			location.Latitude = demandPoint[1];
//			inputPoint.Id = i;
//			inputPoint.PointLocation = location;
//			inputPoint.Population = demandPoint[2];
//			inputPoint.Quality = demandPoint[3];
//		}
//		fclose(f);
//		return p;
//	}
//};
