#pragma once
#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <atomic>

template <typename T>
class ConcurrentQueue
{
private:
	std::queue<T> _queue;
	std::mutex _lock;
	std::atomic<size_t> count;
public:
	ConcurrentQueue() {}
	virtual size_t Count(void);
	virtual T Pop(void);
	virtual bool TryPop(T* out_value);
	virtual void Push(T value);
};