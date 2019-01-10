#pragma once
#include"stdafx.h"
#include"ThreadPool.h"
#include<functional>
#include"TimeManager.h"
#include"GameObject.h"
#include<unordered_map>
#include"Logger.h"
#include<future>

namespace Unbound
{
	/*This class will handle tasks in a multithreaded way. This includes keeping track of tasks that need to be done,
	Removing old tasks that are no longer needed and tracking performance to handle bottlenecks*/
	class TaskScheduler
	{
	public:
		/*The priority of a task determines how soon it will run, and whether it can be dropped*/
		enum TaskPriority { TASK_PRIORITY_LOW, TASK_PRIORITY_NORMAL, TASK_PRIORITY_HIGH };

		/*Events are sent automatically and trigger actions*/
		enum TaskEvent { TASK_EVENT_NONE, TASK_EVENT_PROGRAM_START, TASK_EVENT_LEVEL_LOAD, TASK_EVENT_LEVEL_LOAD_FINISHED, TASK_EVENT_FRAME, TASK_EVENT_LEVEL_UNLOAD, TASK_EVENT_PROGRAM_CLOSE, END };

		/*This class contains a function that will be run at the specified time.
		The first template parameter is the return type of the function, while the second is the argument type
		Normally both are void, however it is possible to make tasks take arguments, even from other tasks*/
		template<typename T = void, typename U = void>
		class Task
		{
		public:
			std::function<T(U)> torun;
			Task() noexcept;
			Task(std::function<T(U)> toset);
			~Task();
		};

		/*This struct wraps a task with its priority and when it should be run. This makes it possible to reuse tasks*/
		struct ScheduledTask
		{
			Task<> task; //The task to run
			TaskPriority priority; //The priority of the task
			Util::Time::TimeStruct when; //The delay for when the task should run. A value of 0 means it will run as soon as possible
			Util::Time::TimeStruct timeAdded; //The time when this task was added to the queue
			bool recurring; //If this is true, then the task will be scheduled to run again as soon as it is run

			ScheduledTask(const Task<>& taskToSet, const TaskPriority& priorityToSet = TASK_PRIORITY_NORMAL, const Util::Time::TimeStruct& triggertime = 0, const Util::Time::TimeStruct& timeadded = 0, const bool isrecurring = false)
			{
				task = taskToSet;
				priority = priorityToSet;
				when = triggertime;
				timeAdded = timeadded;
				recurring = isrecurring;
			}
		};

		//Update the system. This is called every frame and updates the status of each task
		void update();

		//Get the current Task Scheduler instance
		static TaskScheduler* get();

		//Schedule a task to run after a given amount of time in milliseconds(default is asap). Tasks with higher priority run first
		void add(Task<> toAdd, Util::Time::TimeStruct when, TaskPriority priority = TASK_PRIORITY_NORMAL, bool recurring = false);

		//Same as above, but more useful for adding recurring tasks quickly
		void add(std::function<void()> toAdd, bool recurring = false, TaskPriority priority = TaskPriority::TASK_PRIORITY_NORMAL, const std::vector<TaskEvent>& eventsToSubscribeTo = { TASK_EVENT_NONE });

		//Add a gameobject to the task scheduler. This will ensure that each gameobject is updated automatically
		void add(GameObject& toAdd);

		/*Trigger an event to be sent. This is useful for when you create your own events*/
		void triggerEvent(const TaskEvent& toTrigger);

	private:
		std::vector<ScheduledTask> tasks; //All the tasks that are currently registered
		std::vector<std::vector<ScheduledTask>> eventTasks; //Tasks that are triggered by events

		static TaskScheduler* instance; //Current instance
		TaskScheduler() noexcept;
		~TaskScheduler();
	};
}