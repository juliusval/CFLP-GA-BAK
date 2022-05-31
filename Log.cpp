#include "Log.h"
#include "Params.h"
#include <ctime>

using namespace std;

Log Log::Instance;

Log::Log() {}

void Log::Open(const string& logFile)
{
	Instance.logStream.open(logFile.c_str());
}

void Log::Close()
{
	time_t currentTime = time(NULL);
	string timeStr = ctime(&currentTime);
	Log::WriteLine(timeStr, 1);
	Instance.logStream.close();
}

void Log::WriteLine(string text)
{
	Log::WriteLine(text, 2);
}

void Log::WriteLine(string text, int logLevel)
{
	if(!Params::LOG)
		return;
	if (Params::LOG_LEVEL >= logLevel)
	{
		ostream& stream = Instance.logStream;
		stream << text << endl;
		if (logLevel == 0 && Params::RANK == 0)
		{
			cout << text << endl;
		}
	}
}