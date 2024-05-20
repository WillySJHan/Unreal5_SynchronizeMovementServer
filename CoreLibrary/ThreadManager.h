#pragma once

#include <thread>
#include <functional>

class ThreadManager
{
public:
	ThreadManager();
	~ThreadManager();

	void Run(std::function<void(void)> task);
	void Join();

	static void InitTLSVar();
	static void DestroyTLSVar();

	static void ExecuteTask();
	static void PushReservedTask();

private:
	std::mutex			_mutex;
	std::vector<std::thread>	_threads;
};
