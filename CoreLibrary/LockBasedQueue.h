#pragma once

template<typename T>
class LockBasedQueue
{
public:
	void Push(T data)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_queue.push(data);
	}

	T Pop()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		return PopNoLock();
	}

	T PopNoLock()
	{
		if (_queue.empty())
			return T();

		T ret = _queue.front();
		_queue.pop();
		return ret;
	}

	void PopAll(OUT std::vector<T>& datas)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		while (T data = PopNoLock())
			datas.push_back(data);
	}

	void Clear()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_queue = std::queue<T>();
		//_queue.clear();
	}


private:
	std::mutex _mutex;
	std::queue<T> _queue;
};

