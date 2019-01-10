#pragma once
#include"stdafx.h"
#include"AudioListener.h"
#include"AudioSource.h"

namespace Unbound
{
	namespace Audio
	{
		/*A  class to manage and track audio devices and sounds*/
		class AudioManager
		{
		public:
			/*Initialize the audio manager and set up an audio device*/
			void init();
			/*Close the audio manager and clean up any assets that were created*/
			void close();

			/*Adds an audio source to the audio manager for tracking*/
			void add(const AudioSource& toAdd);

			/*Get the current instance of the audiomanager*/
			static AudioManager* get();

			AudioManager();
			~AudioManager();
		private:
			static AudioManager* instance; //The main instance of the audio manager
			//ALCdevice* device; //The device that will be used for audio playback
			//ALCcontext* context; //The context that will be used for audio playback
			std::vector<std::string> deviceNames; //All the audio devices currently in the system
			AudioListener listener; //The main audio listener to represent the listener in the scene

			/*Gets a list of all the audio devices in the system and stores them in devices*/
			void enumerateDevices();
			/*Sets the device that audio will be played from. Usually the default is good, but in some cases it is desirable to change this*/
			void setPlaybackDevice(const std::string& deviceName);
		};
	}
}