#pragma once
#include "stdafx.h"
#include <fstream>

namespace Unbound
{
	namespace IO
	{
		/*A class to manage program settings and serialize them*/
		class SettingsManager
		{
		public:

			static SettingsManager* get(); //Get the current instance
			void loadFile(const std::string& path); //Load the settings ini file located at "path"
			void writeCurrentFile(); //Write the current file to disk. Does not preserve comments

			//Add a setting to the current file. The name should start with the fist letter of the value type, for instance sName is a string and iNumber is an integer
			template <typename T>
			void addSettingToFile(const std::string& settingName, const T value);

			//Get a setting from the current file. Sectionname refers to the section of the ini file to search in. By default, searches are global
			template <typename T>
			const T getSetting(const std::string& key, const T& defaultsetting, std::string sectionname = "ALL");

		private:
			//A single setting, with a name and value (as a string)
			struct Setting
			{
				std::string name; //The name of the setting
				std::string value; //The value of the setting encoded as a string
			};

			//Each section contains certain settings. There can be two settings of the same name in two different sections
			struct SettingSection
			{
				std::string sectionName; //The name of the section
				std::vector<Setting> settings; //All the settings in the section
			};

			//A struct to represent an entire settings file, made up of a number of sections
			struct ConfigSettings
			{
				std::string filePath; //The path to the settings file represented by this structure
				std::vector<SettingSection> sections; //A vector of all the sections in the file. If there are no specific sections, everything gets put into a [MAIN] section
			};

			static SettingsManager* instance;

			std::vector<ConfigSettings> storedSettings; //All the currently stored settings files. Usually only one file is open

			const std::string findSetting(const std::string& name, const std::string& sectionname) const; //Get the string value of a setting in the given section. If sectionname is [ALL], the entire file is searched

			SettingsManager() noexcept;
			~SettingsManager();
		};
	}
}