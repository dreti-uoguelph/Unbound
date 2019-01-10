#pragma once
#include"stdafx.h"
#include<ctime>
#include<sstream>
#include<iomanip>
#include<time.h>

namespace Unbound
{
	namespace Util
	{
		class Time
		{
		public:
			struct TimeStruct
			{
				uint64_t time; //The time expressed as ticks since the start of the application
				time_t macrotime; //The time expressed as seconds since 1970, covers longer period
				const double asSeconds() const; //Get the time in seconds
				const double asMilliseconds() const; //Get the time in milliseconds
				const std::string asTimeAndDate() const; //Get the time as a string containing the time and date representation on your locale

				TimeStruct() noexcept
				{
					time = 0;
					macrotime = 0;
				}

				TimeStruct(const uint64_t initialTime);
				TimeStruct(const uint64_t initialTime, const time_t macroTime);
			};

			static Time* get();

			//Initialize the time handler
			void init();

			//Indicates the start of a new frame. This will update all the per-frame variables, like delta time
			void frame();

			//Get the current time
			const TimeStruct getCurrentTime() const;

			//Get the time passed since the application was started
			const TimeStruct getTimeSinceStart() const;

			//Get the time between the last frame and the current frame
			const TimeStruct getDeltaTime() const;

		private:
			static Time* instance;
			uint64_t currentTime = 0; //The time, as of last frame
			uint64_t oldTime = 0; //The time from the previous frame
			uint64_t deltaTime = 0; //The change in time over the last frame
			uint64_t applicationStartTime = 0; //The time when the application was started
			time_t macroTime = 0; //The time in seconds since 1970

			Time() noexcept;
			~Time();
		};
	}
}