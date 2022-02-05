// FileCrawler.cpp: definiuje punkt wejścia dla aplikacji.
//

#include "FileCrawler.h"
#include <filesystem>

//includes for file read
#include <iostream>
#include <fstream>
#include <string>

#include <thread>

//using namespace std;
namespace filesystem = std::filesystem;

unsigned int countLinesInFile(filesystem::path path)
{
	unsigned int lineCount = 0;
	std::string line;

	//Some checks that could be done: filesystem::exist(path), path.extension()
	//Also is possible to perform check if file is text or binary https://stackoverflow.com/questions/277521/how-to-identify-the-file-content-as-ascii-or-binary

	std::ifstream file(path);
	if (! file.is_open())
	{
		throw "File not opened";
	}

	while (std::getline(file, line))
	{
		lineCount++;
	}
	file.close();

	return lineCount;
}

void countLinesInFileThreadFunc(std::vector<filesystem::path> paths, std::atomic_uint64_t* allFilesLineCount, std::atomic_uint32_t* accesedFileCount)
{
	for (int i = paths.size() - 1; i >= 0; i--)
	{
		unsigned int oneFileLineCount = 0;
		try
		{
			oneFileLineCount = countLinesInFile(paths.at(i));
			(*allFilesLineCount) += oneFileLineCount;
			(*accesedFileCount)++;
		}
		catch (...)
		{
			//TODO: maybe some pointer to fill with not accesible files info
		}
	}
}

uint32_t countFiles(filesystem::path path)
{
	return std::distance(filesystem::recursive_directory_iterator(path), filesystem::recursive_directory_iterator{});
}

void directoryStatisticCounter(filesystem::path path, uint8_t maxThreadCount)
{
	std::atomic_uint64_t lineCount(0);
	std::atomic_uint32_t accesedFileCount(0);

	if (!filesystem::is_directory(path))
	{
		lineCount = countLinesInFile(path);
		accesedFileCount++;
	}
	else
	{
		uint32_t filesPerThread = ceil(countFiles(path) / (double)maxThreadCount);

		std::vector<std::thread> threads;
		std::vector<filesystem::path> pathsForThread;
		int fileNumber = 0;
		bool pathListForThreadFull=true;

		for (const auto& entry : filesystem::recursive_directory_iterator(path))
		{
			fileNumber++;
			pathsForThread.push_back(entry.path());

			pathListForThreadFull = ((filesPerThread == 0) || (fileNumber % filesPerThread == 0));
			if (pathListForThreadFull)
			{
				//Create new thread when files to analize list is full
				threads.push_back(std::thread(&countLinesInFileThreadFunc, pathsForThread, &lineCount, &accesedFileCount));
				pathsForThread.clear();
			}
		}
		if (!pathListForThreadFull)
		{
			//Create thread for leftover files to analize
			threads.push_back(std::thread(&countLinesInFileThreadFunc, pathsForThread, &lineCount, &accesedFileCount));
		}

		for (int i = threads.size()-1; i >= 0; i--)
		{
			threads.at(i).join();
		}
		std::cout << threads.size() << " threads with ~" << filesPerThread << " files per thread - all " << countFiles(path) << std::endl;
	}

	std::cout << lineCount << " lines in " << accesedFileCount << " files with " << std::endl;
}

int main()
{
	//Could be pass by startup args
	char* testPath =  R"(C:\Users\JK\Documents\)";//Arduino\libraries\Complex\Complex.cpp
	int maxThreadCount = 4;
	
	try
	{
		directoryStatisticCounter(testPath, maxThreadCount);
	}
	catch(...)
	{
		std::cout << "Wrong path" << std::endl;
		return -1;
	}
	
	return 0;
}
