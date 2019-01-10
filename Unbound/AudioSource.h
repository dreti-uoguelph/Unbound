#pragma once
#include"stdafx.h"
#include"GameObject.h"
#include"Transform.h"
#include"AudioLoader.h"

namespace Unbound
{
	class AudioSource : public GameObject
	{
	public:
		/*Initializes the audio source*/
		virtual void init() override;
		/*Updates the audio source*/
		virtual void update() override;
		/*Closes the audio source and cleans up associated data*/
		virtual void close() override;

		/*Starts playing the audio in the source from the current position*/
		void play() const;
		/*Starts playing the audio source. If shouldLoop is true, then the audio source will start looping when it reaches the end of the track*/
		void play(const bool shouldPlayLoop);
		/*Pauses the audio source in its current place*/
		void pause() const;

		/*Loads data into the audio source from a file*/
		void load(const std::string& path);
		/*Loads data into the audio source from a file*/
		void load(const std::string& path, const Audio::AudioType typeToLoad);

		/*Sets the volume of the audio source as a percentage*/
		void setVolume(const float toSet);
		/*Returns the current volume of the audio source as a percentage*/
		const float getVolume() const;

		/*Sets whether the audio source should loop*/
		void setShouldLoop(const bool toSet);
		/*Returns the current loop state of the audio source*/
		const bool getShouldLoop() const;

		Transform transform; //The position, rotation and scale of the audio source in world space

		AudioSource() noexcept;
		AudioSource(const std::string& path);
		AudioSource(const std::string& path, const Audio::AudioType type) noexcept;
		~AudioSource();
	protected:
		float volume; //The volume that audio should be played at, as a percentage
		bool shouldLoop; //Whether the audio played from the souce should loop i.e. start over once finished

	private:
		//ALuint buffer; //The buffer holding the data that will be played
		Audio::AudioType type; //The type of audio that this source plays
	};
}