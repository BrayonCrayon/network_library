/*
Jacob Gardiner
0660920

Project One: Process Launcher

*/

/*
include necessary libraries
*/
#include "stdafx.h"
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <vector>
#include <regex>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <sstream>
#include <process.h>
#include <Windows.h>
#include <stdlib.h>
#include <crtdbg.h>
#include <thread>

#define _CRTDBG_MAP_ALLOC  


// declare namespace
using namespace std;


/* Helpers */
bool is_digits(const std::string& str)
{
	return std::all_of(str.begin(), str.end(), ::isdigit); // C++11
}

/*
Process class
*/
class Process
{
private:


public:
	int launchGroup = 0;
	int expectedExitCode = 0;
	int exitCode = 0;

	string programName = "";
	string commandLineParameters = "";
	string invalidProcess = "";

	Process::~Process()
	{
	};


	bool ExpectedExitCalled()
	{
		if (exitCode == expectedExitCode)
		{
			return true;
		}

		return false;
	}
};


/*
Print the contents of a vector of strings
*/
void printVector(vector<string> displayVector)
{
	for (size_t i = 0; i < displayVector.size(); ++i)
	{
		cout << displayVector[i] << endl;
	}
}

/*
Sort the lines from the file into
correnct group order using Bubble
sort
*/
//void ReorderProcesses(vector<string>& strings)
//{
//	typedef vector<string>::size_type size_type;
//	for (size_type i = 1; i < strings.size(); ++i) // for n-1 passes
//	{
//		// In pass i,compare the first n-i elements
//		// with their next elements
//		for (size_type j = 0; j < (strings.size() - 1); ++j)
//		{
//			if (strings[j] > strings[j + 1])
//			{
//				string const temp = strings[j];
//				strings[j] = strings[j + 1];
//				strings[j + 1] = temp;
//			}
//		}
//	}
//}


/*
Complete validation on a line read in from
the file
*/
bool validateLine(string line)
{
	size_t n = count(line.begin(), line.end(), ',');
	if (n == 2)
	{
		return true;
	}

	return false;
}


/*
Read in the contents of a file and return a vector<string>
*/
vector<string> ReadFile(string infile)
{
	string line = "";
	ifstream myfile;
	myfile.open(infile);

	vector<string> lineVector = vector<string>();


	while (getline(myfile, line))
	{
		if (validateLine(line))
		{
			lineVector.push_back(line);
		}
		else
		{
			cout << line << endl;
		}
	}


	
	cout << endl;
	return lineVector;
}


/*
Wait until the group of processes have completed
*/
vector<Process> groupRunning(vector<PROCESS_INFORMATION> pi, vector<Process> processes)
{
	try
	{
		DWORD exitCode = 0;

		for (size_t i = 0; i < pi.size(); ++i)
		{
			WaitForSingleObject(pi[i].hProcess, INFINITE);
			GetExitCodeProcess(pi[i].hProcess, &exitCode);

			int retVal = (int)exitCode;
			if (processes[i].exitCode != 404)
				processes[i].exitCode = retVal;
			string s = "";
		}

	}
	catch (...)
	{
		//	cout << "ERROR" << endl;
	}
	return processes;
}


/*
Print a report on the completed processes
*/
vector<Process> processReports(vector<PROCESS_INFORMATION> pi, vector<Process> processes)
{
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(hStdout, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);

	PROCESS_INFORMATION cpi = { nullptr };

	for (size_t i = 0; i < pi.size(); ++i)
	{
		if (processes[i].ExpectedExitCalled())
		{
			SetConsoleTextAttribute(hStdout, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		}
		else
		{
			SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_INTENSITY);
			continue;
		}

		FILETIME creationTime, exitTime, kernelTime, userTime;
		GetProcessTimes(pi[i].hProcess, &creationTime, &exitTime, &kernelTime, &userTime);

		SYSTEMTIME kTime;
		FileTimeToSystemTime(&kernelTime, &kTime);

		SYSTEMTIME uTime;
		FileTimeToSystemTime(&userTime, &uTime);

		unsigned long long elapsedTicks = *reinterpret_cast<unsigned long long*>(&exitTime) -
			*reinterpret_cast<unsigned long long*>(&creationTime);

		std::size_t found = processes[i].programName.find_last_of("/\\");

		cout << "\t" << processes[i].launchGroup << "\t" << kTime.wHour << ":" << kTime.wMinute << ":" << kTime.wSecond << ":"
			<< kTime.wMilliseconds << " \t"
			<< uTime.wHour << ":" << uTime.wMinute << ":" << uTime.wSecond << ":" << uTime.wMilliseconds <<
			" \t" << processes[i].exitCode << "\t" << setw(20) << processes[i].programName.substr(found + 1) <<
			"\t" << processes[i].commandLineParameters << endl;


		CloseHandle(pi[i].hThread);
		CloseHandle(pi[i].hProcess);
	}
	SetConsoleTextAttribute(hStdout, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
	return processes;
}

void wait()
{
	this_thread::sleep_for(5s);
}


/*
Launch a group of processes and wait until
they have completed
*/
vector<Process> launchGroup(vector<Process> group)
{
	vector<PROCESS_INFORMATION> pi = vector<PROCESS_INFORMATION>();
	for (size_t i = 0; i < group.size(); ++i)
	{
		thread t(wait);
		//this_thread::sleep_for(3s);
		string str = group[i].programName;
		wstring programName(str.length(), L' ');
		copy(str.begin(), str.end(), programName.begin());

		wstring application = L"\"" + programName + L"\"";

		string pstr = group[i].commandLineParameters;
		wstring cmdPar(pstr.length(), L' ');
		copy(pstr.begin(), pstr.end(), cmdPar.begin());

		wstring params = cmdPar;
		wstring command = application + L" " + params;

		STARTUPINFO sinfo = { 0 };

		sinfo.cb = sizeof(STARTUPINFO);

		PROCESS_INFORMATION cpi = { nullptr };
		pi.push_back(cpi);

		unsigned long const CP_MAX_COMMANDLINE = 32768;
		try
		{
			wchar_t* commandline = new wchar_t[CP_MAX_COMMANDLINE];

			wcsncpy_s(commandline, CP_MAX_COMMANDLINE, command.c_str(), command.size() + 1);

			if (CreateProcess(nullptr, // apllication name is null since it is provided in command line
				commandline, // contains the application name and parameters
				nullptr, // same security level as parent (parent is this program)
				nullptr, // start with normal runtime level (can be changed to be high or low priority)
				false, // dont inherit handles
				CREATE_NEW_CONSOLE, // defaults (look up options: homework)
				nullptr,
				nullptr, // Same as parent
				&sinfo,
				&pi.back()
			) == false)
			{
				group[i].exitCode = 404;
				group[i].expectedExitCode = 0;
				//	cout << "Error: Unable to create process" << endl;
			}

			delete[] commandline;
		}
		catch (bad_alloc&)
		{
			wcerr << L"Not enough memory";
		}
		t.join();
		
	}

	//group = groupRunning(pi, group);

	//string s = "";

	//if (group.size() > 0)
	//{
	//	group = processReports(pi, group);
	//}

	return group;
}

/*
Start the processes
*/
vector<Process> startProcesses(vector<Process> processes)
{
	vector<Process> group = vector<Process>();
	vector<Process> returnGroup = vector<Process>();

	for (int i = 0; i <= processes[processes.size() - 1].launchGroup; ++i)
	{
		vector<Process> currentGroup = vector<Process>();


		for (size_t x = 0; x < processes.size(); ++x)
		{
			if (processes[x].launchGroup == i)
			{
				currentGroup.push_back(processes[x]);
			}
		}

		currentGroup = launchGroup(currentGroup);
		for (size_t x = 0; x < currentGroup.size(); ++x)
		{
			returnGroup.push_back(currentGroup[x]);
		}
	}

	return returnGroup;
}

/*
Remove leading and trailing whitespace
from a string
*/
void removeExtraSpaces(string& str)
{
	str = regex_replace(str, regex("^ +"), "");
	str = regex_replace(str, regex("^ +| +$|( ) +"), "$1");
}


Process getExpectedExitCode(string s, Process p)
{
	try
	{
		string result = "";
		string::size_type pos = s.find(' ');

		if (pos != string::npos)
		{
			result = s.substr(0, pos);
			p.expectedExitCode = boost::lexical_cast<int>(result);
			s = s.substr(pos + 1, s.size());
		}
	}
	catch (...)
	{
		//	cout << "CATCH" << endl;
	}
	return p;
}

/*
Extract the launch group from the string
*/
Process getCommandLineArguments(string& s, Process p)
{
	string result = "";
	string::size_type pos = s.size();
	try
	{
		if (pos != string::npos)
		{
			result = s.substr(0, pos);

			if (is_digits(result))
			{
				p.launchGroup = boost::lexical_cast<int>(result);
				s = s.substr(pos + 1, s.size());
				return p;
			}


			p.commandLineParameters = result;
			s = s.substr(pos, s.size());
			removeExtraSpaces(p.commandLineParameters);

			if (is_digits(p.commandLineParameters))
			{
				p = getExpectedExitCode(p.commandLineParameters, p);
			}
			else
			{
				p.expectedExitCode = 0;
			}
		}
	}
	catch (...)
	{
		//cout << "Error:" << endl;
	}




	return p;

}



/*
Extract the launch group from the string
*/
Process getLaunchGroup(string& s, Process p)
{
	try
	{
		string result = "";
		string::size_type pos = s.find(',');

		if (pos != string::npos)
		{
			result = s.substr(0, pos);


			if (is_digits(result))
			{
				p.launchGroup = boost::lexical_cast<int>(result);
				s = s.substr(pos + 1, s.size());
				return p;
			}

			throw std::invalid_argument("Launch group is non numeric");
		}
	}
	catch (exception e)
	{
		//cout << "Error:" << e.what() << endl;
		p.invalidProcess = s;
		p.launchGroup = 500;
		p.exitCode = 500;
		p.expectedExitCode = 50;
		p.programName = "Invalid Launch Group";
	}
	return p;
}


/*
Extract the program name from the string
*/
Process getProgramName(string& s, Process p)
{
	try
	{
		string result = "";
		string::size_type pos = s.find(',');

		if (pos != string::npos)
		{
			result = s.substr(0, pos);
			p.programName = result;
			s = s.substr(pos + 1, s.size());
			removeExtraSpaces(p.programName);
			

			return p;
		}
	}
	catch (exception e)
	{
		//	cout << "Error:" << e.what() << endl;
		p.invalidProcess = s;
		p.launchGroup = 500;
		p.exitCode = 500;
		p.expectedExitCode = 50;
		p.programName = "Invalid Program Name";
	}
	return p;
}


/*
Convert an array of strings from the file to
an array of Process objects
*/
vector<Process> convertToProcess(vector<string> strings)
{
	vector<Process> processes = vector<Process>();
	try
	{

		for (size_t i = 0; i < strings.size(); ++i)
		{
			Process temp;
			try
			{


				temp = getLaunchGroup(strings[i], temp);

				if (temp.invalidProcess != "")  throw std::invalid_argument("Launch group is non numeric");

				temp = getProgramName(strings[i], temp);
				temp = getCommandLineArguments(strings[i], temp);
				temp = getExpectedExitCode(temp.commandLineParameters, temp);
				processes.push_back(temp);
			}
			catch (...)
			{
				processes.push_back(temp);
			}
		}

	}
	catch (...)
	{
		//cout << "Unable to Convert to Process" << endl;
	}
	return processes;
}


class ProcessLauncher
{
private:

	vector<string> _lines = vector<string>();
	vector<string> _invalidProcesses = vector<string>();
public:
	vector<Process> _processes = vector<Process>();

	ProcessLauncher::ProcessLauncher(string fileName = "example.txt")
	{
		// Read in the contents of the file
		_lines = ReadFile(fileName);
	};

	void PrintFailedProcesses()
	{
		HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_INTENSITY);

		//cout << "********* Failure Report *********" << endl;

		for (size_t i = 0; i < _processes.size(); ++i)
		{
			if (_processes[i].exitCode != _processes[i].expectedExitCode)
			{
				std::size_t found = _processes[i].programName.find_last_of("/\\");
				cout << "\t" << _processes[i].launchGroup << "\t" << "00:00:00" << "\t" << "00:00:00" << "\t" << _processes[i].
					exitCode << "\t" << setw(20) << _processes[i].programName.substr(found + 1) <<
					"\t" << _processes[i].commandLineParameters << endl;
			}
		}
		for (size_t i = 0; i < _invalidProcesses.size(); ++i)
		{
			cout << _invalidProcesses[i] << endl;
		}
	}

	void StartProcesses()
	{
		try
		{
			HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);


			SetConsoleTextAttribute(hStdout, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
			cout <<
				"**************************************************************************************************************" <<
				endl;
			cout <<
				"*                                             ProcessLauncher                                                *" <<
				endl;
			cout <<
				"**************************************************************************************************************" <<
				endl;

			cout << "Launch Group\tKernel Time\tUser Time\tExit Code\tProgram Name\tCMD Parameters" << endl;
			cout << "-------------------------------------------------------------------------------------------------------" <<
				endl;

			// sort the processes 
			//ReorderProcesses(_lines);
			_processes = convertToProcess(_lines);

			//	printVector(_lines);
			_processes = startProcesses(_processes);

			string s = "";

			//	_CrtDumpMemoryLeaks();
			PrintFailedProcesses();
		}
		catch (...)
		{
			cout << "Failed to start processes" << endl;
		}
	}
};


int main()
{
	ProcessLauncher test = ProcessLauncher("launch_commands.txt");
	test.StartProcesses();

	cout << "Processes Complete" << endl;
	string s = "";
	cin >> s;
}
