#pragma once

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class Log
{
public:
	static void Open(const string& logFile);
	static void Close();
	static void WriteLine(string text);
	static void WriteLine(string text, int logLevel);

private:
	Log();
	ofstream logStream;
	static Log Instance;
};

