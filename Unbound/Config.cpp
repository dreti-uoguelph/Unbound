#include "stdafx.h"
#include "Config.h"

namespace Unbound
{
	namespace IO
	{
		SettingsManager* SettingsManager::instance = nullptr;

		template<typename T>
		void SettingsManager::addSettingToFile(const std::string& settingName, T value)
		{
		}

		template<>
		void SettingsManager::addSettingToFile(const std::string& settingName, std::string value)
		{
			//storedSettings[0].sections[0].settings.push_back({ settingName, value });
			//writeCurrentFile();
		}

		template<typename T>
		const T SettingsManager::getSetting(const std::string& key, const T& defaultsetting, std::string sectionname)
		{
		}

		template<>
		const std::string SettingsManager::getSetting(const std::string& key, const std::string& defaultsetting, std::string sectionname)
		{
			//Add the square braces to the section name before searching for it
			sectionname = sectionname.insert(0, "[");
			sectionname.push_back(']');

			//Find the string representation of the integer
			std::string val = instance->findSetting(key, sectionname);

			if (val != "")
			{
				return val;
			}

			//If the setting was not found, add the default value to the file
			addSettingToFile<std::string>(key, defaultsetting);
			return defaultsetting;
		}

		template<>
		const int SettingsManager::getSetting(const std::string& key, const int& defaultsetting, std::string sectionname)
		{
			//Add the square braces to the section name before searching for it
			sectionname = sectionname.insert(0, "[");
			sectionname.push_back(']');

			//Find the string representation of the integer
			std::string val = instance->findSetting(key, sectionname);

			if (val != "")
			{
				return atoi(val.c_str());
			}

			//If the setting was not found, add the default value to the file
			addSettingToFile<int>(key, defaultsetting);
			return defaultsetting;
		}

		template<>
		const bool SettingsManager::getSetting(const std::string& key, const bool& defaultsetting, std::string sectionname)
		{
			//Add the square braces to the section name before searching for it
			sectionname = sectionname.insert(0, "[");
			sectionname.push_back(']');

			//Find the string representation of the integer
			std::string val = instance->findSetting(key, sectionname);

			if (val != "")
			{
				if (val == "true" || val == "t" || val == "T" || val == "TRUE" || val == "True")
				{
					return true;
				}

				return false;
			}

			//If the setting was not found, add the default value to the file
			addSettingToFile<bool>(key, defaultsetting);
			return defaultsetting;
		}

		template<>
		const float SettingsManager::getSetting(const std::string& key, const float& defaultsetting, std::string sectionname)
		{
			//Add the square braces to the section name before searching for it
			sectionname = sectionname.insert(0, "[");
			sectionname.push_back(']');

			//Find the string representation of the integer
			std::string val = instance->findSetting(key, sectionname);

			if (val != "")
			{
				return strtof(val.c_str(), NULL);
			}

			//If the setting was not found, add the default value to the file
			addSettingToFile<float>(key, defaultsetting);
			return defaultsetting;
		}

		SettingsManager* SettingsManager::get()
		{
			if (instance == nullptr)
			{
				instance = new SettingsManager();
			}

			return instance;
		}

		void SettingsManager::loadFile(const std::string& path)
		{
			std::ifstream inFile; //The handle to the current input file
			std::string line; //The current line in the file
			SettingSection currentSection; //The section currently being loaded
			Setting currentSetting; //The setting currently being loaded
			ConfigSettings loadedSettings; //The settings that were loaded from the file

			inFile.open(path, std::ios::in);

			if (!inFile.good())
			{
				return;
			}

			loadedSettings.filePath = path;

			while (getline(inFile, line))
			{
				//If the line starts with [, it indicates a new section
				if (line[0] == '[')
				{
					//Add the old section to the loaded settings
					if (currentSection.settings.size() > 0)
					{
						loadedSettings.sections.push_back(currentSection);
						currentSection.settings.clear();
					}

					currentSection.sectionName = line;

					continue;
				}

				//Make sure the line is actually valid
				if (line.find('=') != std::string::npos)
				{
					//The name of the setting comes before the = sign
					currentSetting.name = line.substr(0, line.find('='));

					//The value of the setting comes after
					currentSetting.value = line.substr(line.find('=') + 1, line.back());

					currentSection.settings.push_back(currentSetting);
				}
			}

			//If there are no explicit sections defined in the file, put everything in the main section
			if (currentSection.sectionName == "")
			{
				currentSection.sectionName = "[MAIN]";
			}

			loadedSettings.sections.push_back(currentSection);
			instance->storedSettings.push_back(loadedSettings);

			inFile.close();
		}

		void SettingsManager::writeCurrentFile()
		{
			std::ofstream outFile;
			outFile.open(storedSettings[0].filePath, std::ios::out);

			if (!outFile.good())
			{
				return;
			}

			//Loop through each section and write it to the file
			for (auto i : storedSettings[0].sections)
			{
				//Section name comes first
				outFile << i.sectionName << std::endl;

				for (auto j : i.settings)
				{
					//Each section name and value is separated by and = sign (no spaces)
					outFile << j.name << "=" << j.value << std::endl;
				}
			}

			outFile.close();
		}

		const std::string SettingsManager::findSetting(const std::string& name, const std::string& sectionname) const
		{
			//Loop through all files and all sections to look for the given name
			for (auto i : instance->storedSettings)
			{
				for (auto j : i.sections)
				{
					//If the current section is not the section being requested, move to the next one
					if (sectionname != "[ALL]" && j.sectionName != sectionname)
					{
						continue;
					}

					for (auto k : j.settings)
					{
						if (k.name == name)
						{
							return k.value;
						}
					}
				}
			}

			return "";
		}

		SettingsManager::SettingsManager() noexcept
		{
		}

		SettingsManager::~SettingsManager()
		{
		}

		std::string to_string(std::string arg)
		{
			return arg;
		}
	}
}