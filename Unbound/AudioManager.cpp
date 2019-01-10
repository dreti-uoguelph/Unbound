#include"stdafx.h"
#include"AudioManager.h"

namespace Unbound
{
	namespace Audio
	{
		AudioManager* AudioManager::instance = nullptr;

		/*Initialize the audio manager and set up an audio device*/
		void AudioManager::init()
		{
		}

		/*Close the audio manager and clean up any assets that were created*/
		void AudioManager::close()
		{
		}

		/*Adds an audio source to the audio manager for tracking*/
		void AudioManager::add(const AudioSource & toAdd)
		{
		}

		/*Get the current instance of the audiomanager*/
		AudioManager* AudioManager::get()
		{
			if (instance == nullptr)
			{
				instance = new AudioManager();
			}

			return instance;
		}

		AudioManager::AudioManager()
		{
		}

		AudioManager::~AudioManager()
		{
			close();
		}

		/*Gets a list of all the audio devices in the system and stores them in devices*/
		void AudioManager::enumerateDevices()
		{
		}

		/*Sets the device that audio will be played from. Usually the default is good, but in some cases it is desirable to change this*/
		void AudioManager::setPlaybackDevice(const std::string & deviceName)
		{
		}
	}
}