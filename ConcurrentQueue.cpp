#include "ConcurrentQueue.h"

template <typename T>
size_t ConcurrentQueue<T>::Count(void)
{
	// it was unclear by the docs whether queue was thread safe
	// so I just rolled out my own atmomic counter
	std::lock_guard<std::mutex> lock(_lock);

	return count.load(std::memory_order_seq_cst);
}

template <typename T>
T ConcurrentQueue<T>::Pop(void)
{
	count.fetch_sub(1);

	std::lock_guard<std::mutex> lock(_lock);

	T result = _queue.front();

	_queue.pop();

	return result;
}

template <typename T>
void ConcurrentQueue<T>::Push(T value)
{
	count.fetch_add(1);

	std::lock_guard<std::mutex> lock(_lock);

	_queue.push(value);
}