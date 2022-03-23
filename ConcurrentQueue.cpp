#include "ConcurrentQueue.h"

template <typename T>
size_t ConcurrentQueue<T>::Count(void)
{
	// it was unclear by the docs whether queue was thread safe
	// so I just rolled out my own atmomic counter
	return count.load();
}

template <typename T>
T ConcurrentQueue<T>::Pop(void)
{
	std::unique_lock<std::mutex> lock(_lock);

	count.fetch_sub(1);

	T result = _queue.front();

	_queue.pop();

	return result;
}

template <typename T>
void ConcurrentQueue<T>::Push(T value)
{
	std::unique_lock<std::mutex> lock(_lock);

	count.fetch_add(1);

	_queue.push(value);
}