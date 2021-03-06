#include "stdafx.h"
#ifndef STBI
#define STBI
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"
#endif

using std::cout;
using std::endl;
using std::cin;
using std::string;
using std::vector;
using std::fstream;

/*Prints the help message to show this program's usage to users*/
void printHelp();

int main(int argc, char** argv)
{
	enum ConversionMode { CONVERT_FILE, CONVERT_DIRECTORY };
	ConversionMode mode; //The conversion mode that this tool will work in
	string pathToOpen; //The path to the input image or directory
	string outputPath; //The path to the output file or directory

	if (argc == 1)
	{
		bool fileValid = false;

		cout << "This tool compresses images to game compatible formats" << endl;
		cout << "Enter 1 to convert a file, 2 to convert a directory or h for help: ";
		string md;
		cin >> md;

		if (md == "h")
		{
			printHelp();
			cin.get();
			cin.get();
			exit(0);
		}
		else
		{
			mode = static_cast<ConversionMode>(strtol(md.c_str(), nullptr, 10) - 1);
		}

		while (!fileValid)
		{
			cout << "Enter path to input texture or directory: ";
			cin >> pathToOpen;

			if (mode == CONVERT_FILE)
			{
				//Check the path that was entered to verify its validity
				fstream file;
				file.open(pathToOpen, std::ios::in);

				if (file.is_open())
				{
					file.close();
					fileValid = true;
				}
				else
				{
					cout << "Error: could not open file at " << pathToOpen.c_str() << " the file may not exist or you may not have permission to open it" << endl;
				}
			}
			else
			{
				fileValid = true;
			}
		}

		fileValid = false;
		while (!fileValid)
		{
			cout << "Enter path to output texture or directory: ";
			cin >> pathToOpen;
			fileValid = true;
		}
	}
	else
	{
		//Print usage and help instructions
		if (strcmp(argv[0], "--help") == 0 || strcmp(argv[0], "-h") == 0)
		{
			printHelp();
		}

		for (int i = 0; i < argc; i++)
		{
			if (strcmp(argv[i], "--convert-dir") == 0)
			{
			}
			else if (strcmp(argv[i], "--convert-missing") == 0)
			{
			}
			else if (strcmp(argv[i], "--output-dxt1") == 0)
			{
			}
			else if (strcmp(argv[i], "--output-dxt5") == 0)
			{
			}
			else if (strcmp(argv[i], "--output-lossless") == 0)
			{
			}
			else
			{
				if (pathToOpen == "")
				{
					pathToOpen = string(argv[i]);
				}
				else if (outputPath == "")
				{
					outputPath = string(argv[i]);
				}
				else
				{
					cout << "Error: unknown command: " << argv[i] << endl;
					cin.get();
					exit(0);
				}
			}
		}
	}

	return 0;
}

void printHelp()
{
	cout << endl;
	cout << "This is a tool for compression images from common formats into game specific formats" << endl;
	cout << "Currently, the input formats can be: png psd tga jpg gif bmp hdr pic" << endl;
	cout << "The output formats supported are: .t" << endl;
	cout << "Output encoding: DXT1 for low quality image" << endl;
	cout << "DXT5 for high quality images" << endl;
	cout << "TIFF for lossless images" << endl;
	cout << "" << endl << endl;
	cout << "Usage: UnboundTextureCompiler <flags> input_path output_path" << endl << endl;
	cout << "Supported flags: --convert-dir converts every file in the input directory" << endl;
	cout << "--convert-missing only converts image files which do not have a .t file in the same dir" << endl;
	cout << "--output-dxt1" << endl;
	cout << "--output-dxt5" << endl;
	cout << "--output-lossless" << endl;
	cout << endl;
}