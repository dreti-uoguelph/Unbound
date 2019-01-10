#include"stdafx.h"
#include"AudioSource.h"

namespace Unbound
{
	/*Initializes the audio source*/
	void AudioSource::init()
	{
	}

	/*Updates the audio source*/
	void AudioSource::update()
	{
	}

	/*Closes the audio source and cleans up associated data*/
	void AudioSource::close()
	{
	}

	/*Starts playing the audio in the source from the current position*/
	void AudioSource::play() const
	{
	}

	/*Starts playing the audio source. If shouldLoop is true, then the audio source will start looping when it reaches the end of the track*/
	void AudioSource::play(const bool shouldPlayLoop)
	{
		shouldLoop = shouldPlayLoop;
		play();
	}

	/*Pauses the audio source in its current place*/
	void AudioSource::pause() const
	{
	}

	/*Loads data into the audio source from a file*/
	void AudioSource::load(const std::string& path)
	{
		Audio::AudioLoader loader = Audio::AudioLoader(path);
	}

	/*Loads data into the audio source from a file*/
	void AudioSource::load(const std::string& path, const Audio::AudioType typeToLoad)
	{
		Audio::AudioLoader loader = Audio::AudioLoader(path, typeToLoad);
	}

	/*Sets the volume of the audio source as a percentage*/
	void AudioSource::setVolume(const float toSet)
	{
		volume = toSet;
	}

	/*Returns the current volume of the audio source as a percentage*/
	const float AudioSource::getVolume() const
	{
		return volume;
	}

	/*Sets whether the audio source should loop*/
	void AudioSource::setShouldLoop(const bool toSet)
	{
		shouldLoop = toSet;
	}

	/*Returns the current loop state of the audio source*/
	const bool AudioSource::getShouldLoop() const
	{
		return shouldLoop;
	}

	AudioSource::AudioSource() noexcept
	{
		shouldLoop = false;
		volume = 100;
		init();
	}

	AudioSource::AudioSource(const std::string& path)
	{
		shouldLoop = false;
		volume = 100;
		init();

		//Load audio from the specified file
		load(path);
	}

	AudioSource::AudioSource(const std::string& path, const Audio::AudioType type) noexcept
	{
		shouldLoop = false;
		volume = 100;
		init();

		//Load audio from the specified file
		load(path, type);
	}

	AudioSource::~AudioSource()
	{
		close();
	}
}