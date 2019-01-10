#pragma once
#include"stdafx.h"
#include<phonon.h>

namespace Unbound
{
	namespace Audio
	{
		enum AudioType { AUDIO_TYPE_INMEMORY, AUDIO_TYPE_STREAMED };

		class AudioLoader
		{
		public:

			/*Loads audio from the file specified by path into memory*/
			void load(const std::string& path);
			/*Loads audio from a file specified by path with a type specified by type*/
			void load(const std::string& path, const AudioType& type);

			/*Returns a pointer to the raw data loaded from an audio file*/
			const short* getRawData() const;

			AudioLoader() noexcept;
			AudioLoader(const std::string& path) noexcept;
			AudioLoader(const std::string& path, const AudioType& type);
			~AudioLoader();
		private:
			short* rawData; //The raw data loaded from the audio file
			int numChannels; //The number of channels in the audio file
			int sampleRate; //The sampling rate of the audio file
		};
	}
}