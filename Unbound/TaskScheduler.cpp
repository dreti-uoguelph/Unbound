#include "stdafx.h"
#include "TaskScheduler.h"

namespace Unbound
{
	TaskScheduler* TaskScheduler::instance = nullptr;

	//Update the system. This is called every frame and updates the status of each task
	void TaskScheduler::update()
	{
		auto currentTime = Util::Time::get()->getCurrentTime();

		for (int i = 0; i < tasks.size(); i++)
		{
			if ((currentTime.asSeconds() - tasks[i].timeAdded.asSeconds()) >= tasks[i].when.asSeconds())
			{
				if (tasks[i].priority != TaskPriority::TASK_PRIORITY_HIGH)
				{
					std::async(tasks[i].task.torun);
				}
				else
				{
					tasks[i].task.torun();
				}

				if (!tasks[i].recurring)
				{
					tasks.erase(tasks.begin() + i);
				}
				else
				{
					tasks[i].timeAdded = Util::Time::get()->getCurrentTime();
				}
			}
		}
	}

	//Get the current Task Scheduler instances
	TaskScheduler* TaskScheduler::get()
	{
		if (instance == nullptr)
		{
			instance = new TaskScheduler();
		}

		return instance;
	}

	/*Schedule a task to run after a given amount of time (default is asap). Tasks with higher priority run first
	  Time is in milliseconds and tasks with a lower time will run first. Tasks set to run at the same time will be
	  Sorted based on priority*/
	void TaskScheduler::add(Task<> toAdd, Util::Time::TimeStruct when, TaskPriority priority, bool recurring)
	{
		tasks.push_back({ toAdd, priority, (when.time * CLOCKS_PER_SEC) / 1000, Util::Time::get()->getCurrentTime(), recurring });
	}

	//Same as above, but more useful for adding recurring tasks quickly
	void TaskScheduler::add(std::function<void()> toAdd, bool recurring, TaskPriority priority, const std::vector<TaskEvent>& eventToSubscribeTo)
	{
		if (eventToSubscribeTo[0] != TASK_EVENT_NONE)
		{
			for (auto i : eventToSubscribeTo)
			{
				eventTasks[i].push_back({ { toAdd }, priority, 0, Util::Time::get()->getCurrentTime(), recurring });
			}

			return;
		}

		tasks.push_back({ { toAdd }, priority, 0, Util::Time::get()->getCurrentTime(), recurring });
	}

	//Adds a gameobject, which will be updated automatically
	void TaskScheduler::add(GameObject& toAdd)
	{
		//Add the init function to run as soon as possible
		//tasks.push_back({ [=]() {toAdd.init(); }, TASK_PRIORITY_HIGH, static_cast<time_t>(0.0), Util::Time::get()->getCurrentTime(), false });

		//Set the update function to run at every update interval
		//tasks.push_back({ { std::function<void()>(&toAdd.update) }, TASK_PRIORITY_NORMAL, toAdd.getUpdateInterval(), Util::Time::get()->getCurrentTime(), true });
	}

	/*Trigger an event to be sent. This is useful for when you create your own events*/
	void TaskScheduler::triggerEvent(const TaskEvent& toTrigger)
	{
		for (auto i : eventTasks[toTrigger])
		{
			tasks.push_back(i);
		}
	}

	TaskScheduler::TaskScheduler() noexcept
	{
		tasks.reserve(128);

		for (uint32_t i = 0; i < TaskEvent::END - 1; i++)
		{
			eventTasks.push_back(std::vector<ScheduledTask>());
			eventTasks.back().reserve(64);
		}
	}

	TaskScheduler::~TaskScheduler()
	{
	}

	template<typename T, typename U>
	TaskScheduler::Task<T, U>::Task() noexcept
	{
	}

	template<typename T, typename U>
	TaskScheduler::Task<T, U>::Task(std::function<T(U)> toset)
	{
		torun = toset;
	}

	template<typename T, typename U>
	TaskScheduler::Task<T, U>::~Task()
	{
	}
}