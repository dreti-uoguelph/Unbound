#include "stdafx.h"
#include "CompileModel.h"

int main(int argc, char** argv)
{
	int numThreads = 4; //The number of threads that will be converting files simultaneously
	int numErrors = 0; //The number of errors that occured during model compilation
	vector<string> filePaths(argc - 1); //The paths of all the files to convert

	//If the program is being run from the window manager, ask for inputs before starting
	if (argc == 1)
	{
		cout << "Enter the path to the model that you want to convert: ";
		string path;
		cin >> path;
		filePaths.push_back(path);
#ifdef _WIN32
		system("cls");
#endif
	}

	//Get the number of threads to use, if supplied in the command line
	for (int i = 0; i < argc - 1; i++)
	{
		if (strncmp(argv[i], "--numthreads ", 13) == 0)
		{
			numThreads = atoi(argv[13]);
			continue;
		}

		filePaths[i] = argv[i];
	}

	cout << "Starting Model Compiler with " << numThreads << " threads and " << filePaths.size() << " model(s)" << endl;
	vector<thread> threads(numThreads);

	//Initialize all the threads to work on converting one model
	unsigned int j = 0;
	for (auto i : filePaths)
	{
		if (j == numThreads)
		{
			j = 0;
		}

		threads[j] = thread(compileModel, i, j, &numErrors);
		j++;
	}

	//Wait for all the threads to finish before closing the program
	for (int i = 0; i < threads.size(); i++)
	{
		if (threads[i].joinable())
		{
			threads[i].join();
		}
	}

	cout << endl << "Successfuly compiled " << filePaths.size() << " model(s)" << " with " << numErrors << " error(s)" << endl;

	if (numErrors > 0)
	{
#ifdef _WIN32
		system("pause");
#endif
	}

	return 0;
}