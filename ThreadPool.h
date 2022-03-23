#pragma once
#include <iostream>
#include <thread>
#include <atomic>
#include "ConcurrentQueue.h"
#include <concepts>

class SentinelToken {
private:
	std::atomic<bool> flag_cancelled;
public:
	bool IsCancelled() {
		return flag_cancelled.load();
	}
	void Cancel()
	{
		flag_cancelled.store(true);
	}
};

/// <summary>
/// Defines a discrete piece of work that can be performed on an arbitrary worker thread
/// </summary>
class WorkItem {
public:
	virtual bool PerformWork(SentinelToken& token) = 0;
};

template <typename T>
requires std::derived_from<T, WorkItem>
class ThreadPool
{
private:
	ConcurrentQueue<T> workQueue;
	SentinelToken cancellationToken;
	std::atomic<size_t> threadCount = 0;
	std::vector<std::thread> threads;
	size_t maxThreads;
	std::mutex _locker;
	void WorkConsumer(SentinelToken& token);
public:
	ThreadPool(size_t maxThreads) : maxThreads(maxThreads), threads(maxThreads) {}
	/// <summary>
	/// Enqueues the provided work to be performed on worker threads, ensures FIFO order
	/// </summary>
	virtual void EnqueueWork(T work);
	/// <summary>
	/// Starts the maximum number of worker threads to begin consuming work from the work qeue
	/// </summary>
	virtual void PerformWork();
	/// <summary>
	/// Non Blocking;Sends cancellation signal to all active worker threads
	/// </summary>
	/// <typeparam name="T"></typeparam>
	virtual void CancelWork();
	/// <summary>
	/// Blocks the current execution thread until either all work has finished, or all work has sucessfully been cancelled.
	/// </summary>
	void Wait();
	~ThreadPool();
};

