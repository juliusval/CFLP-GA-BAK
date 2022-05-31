#pragma once

void PrintInputPoints(std::vector<InputPoint>& inputPoints);

void SetInputVectors(std::vector<InputPoint>& inputPoints, std::vector<DemandPoint>& dp, std::vector<Facility>& pf, std::vector<Facility>& cf);

void ParseSettingsJson();
