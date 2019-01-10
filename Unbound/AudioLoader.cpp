#include "stdafx.h"
#include "AudioLoader.h"
#include <stb_vorbis.c>

namespace Unbound
{
	namespace Audio
	{
		/*Loads audio from the file specified by path into memory*/
		void AudioLoader::load(const std::string& path)
		{
			//This will load the entire file into memory and get it ready for playing
			stb_vorbis_decode_filename(path.c_str(), &numChannels, &sampleRate, &rawData);
		}

		/*Loads audio from a file specified by path with a type specified by type*/
		void AudioLoader::load(const std::string& path, const AudioType& type)
		{
			if (type == AUDIO_TYPE_INMEMORY)
			{
				load(path);
				return;
			}

			//Prepare the file for streaming
		}

		/*Returns a pointer to the raw data loaded from an audio file*/
		const short* AudioLoader::getRawData() const
		{
			return rawData;
		}

		AudioLoader::AudioLoader() noexcept
		{
			rawData = nullptr;
			numChannels = 2;
			sampleRate = 48;
		}
		AudioLoader::AudioLoader(const std::string& path) noexcept
		{
			rawData = nullptr;
			numChannels = 2;
			sampleRate = 48;
		}
		AudioLoader::AudioLoader(const std::string& path, const AudioType& type)
		{
			rawData = nullptr;
			numChannels = 2;
			sampleRate = 48;
		}
		AudioLoader::~AudioLoader()
		{
			delete[] rawData;
		}
	}
}