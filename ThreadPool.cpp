#include "ThreadPool.h"

template <typename T>
requires std::derived_from<T, WorkItem>
void ThreadPool<T>::EnqueueWork(T work)
{
	// ConcurrentQueue is thread safe
	workQueue.Push(work);
}

template <typename T>
requires std::derived_from<T, WorkItem>
void ThreadPool<T>::WorkConsumer(SentinelToken& token)
{
	// only continue consuming if there is work to be done
	// AND we have not been cancelled yet
	while ((token.IsCancelled() == false) && workQueue.Count())
	{
		// grab some work
		// ConcurrentQueue is thread safe
		// pop is blocking
		T item = workQueue.Pop();

		// cast it to it's base class
		WorkItem* work = &item;

		// if we failed to perform the work cancel all other jobs
		if(work->PerformWork(token) == false)
		{ 
			token.Cancel();
		}
	}

	// sice this execution thread is about to finish
	// decrement the amount of active threads
	threadCount.fetch_sub(1);
}


template <typename T>
requires std::derived_from<T, WorkItem>
void ThreadPool<T>::PerformWork()
{
	if (workQueue.Count())
	{
		// just incase this is called at the time
		// time as another thread we dont want to create
		// more threads than allowed
		std::lock_guard<std::mutex> lock(_locker);

		auto consumer = [this](SentinelToken& token) {
			WorkConsumer(token);
		};
		while (threadCount.fetch_add(1) < maxThreads) {
			threads.push_back(std::thread(consumer, std::ref(cancellationToken)));
		}
	}
}

template <typename T>
requires std::derived_from<T, WorkItem>
void ThreadPool<T>::CancelWork()
{
	cancellationToken.Cancel();
}

template <typename T>
requires std::derived_from<T, WorkItem>
void ThreadPool<T>::Wait()
{
	std::lock_guard<std::mutex> lock(_locker);
	for (auto &thread : threads)
	{
		if (thread.joinable())
		{
			thread.join();
		}
	}
}

template <typename T>
requires std::derived_from<T, WorkItem>
ThreadPool<T>::~ThreadPool()
{
	// cancel all work if for some reason we have active threads
	CancelWork();

	// wait for all threads to finish execution
	Wait();
}