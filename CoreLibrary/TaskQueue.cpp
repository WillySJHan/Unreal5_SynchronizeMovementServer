#include "pch.h"
#include "TaskQueue.h"
#include "TaskQueueManager.h"

void TaskQueue::Push(std::shared_ptr<Task> task, bool pushOnly)
{
	const int32_t prevCount = _taskCount.fetch_add(1);
	_taskQueue.Push(task); 

	
	if (prevCount == 0)
	{
		
		if (t_myTaskQueue == nullptr)
		{
			Execute();
		}
		else
		{
			g_taskQueueManager->Push(shared_from_this());
		}
	}
}

void TaskQueue::Execute()
{
	t_myTaskQueue = this;

	while (true)
	{
		std::vector<std::shared_ptr<Task>> tasks;
		_taskQueue.PopAll(tasks);

		const int32_t taskCount = static_cast<int32_t>(tasks.size());
		for (int32_t i = 0; i < taskCount; i++)
			tasks[i]->Execute();

		if (_taskCount.fetch_sub(taskCount) == taskCount)
		{
			t_myTaskQueue = nullptr;
			return;
		}

		const uint64_t now = ::GetTickCount64();
		if (now >= t_endTime)
		{
			t_myTaskQueue = nullptr;
			g_taskQueueManager->Push(shared_from_this());
			break;
		}
	}
}
