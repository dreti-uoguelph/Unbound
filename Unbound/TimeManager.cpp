#include "stdafx.h"
#include "TimeManager.h"

namespace Unbound
{
	namespace Util
	{
		Time* Time::instance = nullptr;

		Time* Time::get()
		{
			if (instance == nullptr)
			{
				instance = new Time();
				instance->init();
			}

			return instance;
		}

		void Time::init()
		{
			frame();
		}

		//Indicates the start of a new frame. This will update all the per-frame variables, like delta time
		void Time::frame()
		{
			oldTime = currentTime;
			currentTime = static_cast<uint64_t>(clock());
			deltaTime = currentTime - oldTime;
			macroTime = time(NULL);
		}

		//Get the current time
		const Time::TimeStruct Time::getCurrentTime() const
		{
			return { currentTime, macroTime };
		}

		/*Get the time passed since the application was started*/
		const Time::TimeStruct Time::getTimeSinceStart() const
		{
			return { getCurrentTime().time - applicationStartTime, macroTime };
		}

		//Get the time between the last frame and the current frame
		const Time::TimeStruct Time::getDeltaTime() const
		{
			return { deltaTime, macroTime };
		}

		Time::Time() noexcept
		{
		}

		Time::~Time()
		{
		}

		const double Time::TimeStruct::asSeconds() const
		{
			return static_cast<double>(time) / CLOCKS_PER_SEC;
		}
		const double Time::TimeStruct::asMilliseconds() const
		{
			return static_cast<double>(time * 1000) / CLOCKS_PER_SEC;
		}
		const std::string Time::TimeStruct::asTimeAndDate() const
		{
			tm* tstruct = new tm;
			localtime_s(tstruct, &macrotime);
			std::ostringstream oss;
			oss << std::put_time(tstruct, "%c");
			return oss.str();
		}

		Time::TimeStruct::TimeStruct(const uint64_t initialTime)
		{
			time = initialTime;
			macrotime = 0;
		}

		Time::TimeStruct::TimeStruct(const uint64_t initialTime, const time_t macroTime)
		{
			time = initialTime;
			macrotime = macroTime;
		}
	}
}