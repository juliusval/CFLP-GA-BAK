#include "DataHelper.h"
#include "Params.h"

vector<InputPoint> DataHelperFile::FetchPoints()
	{
		vector<InputPoint> p;
		FILE* f;
		f = fopen("demandPointsRandomQuality.dat", "r");
		for (int i = 0; i < Params::DP; i++)
		{
			double* demandPoint = new double[4];
			fscanf(f, "%lf%lf%lf%lf", &demandPoint[0], &demandPoint[1], &demandPoint[2], &demandPoint[3]);
			InputPoint inputPoint;
			Location location;
			location.Longitude = demandPoint[0];
			location.Latitude = demandPoint[1];
			location.ID = i;
			inputPoint.Id = i;
			inputPoint.PointLocation = location;
			inputPoint.Population = demandPoint[2];
			inputPoint.Quality = demandPoint[3];
			p.push_back(inputPoint);
		}
		fclose(f);
		return p;
	}