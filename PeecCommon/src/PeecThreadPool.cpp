#include "PeecThreadPool.hpp"

Pool::ThreadPool::ThreadPool(std::uint8_t _numThread)
{
	threads.reserve(_numThread);
	for (std::uint8_t i = 0; i < _numThread; i++)
	{
		threads.emplace_back(&ThreadPool::WaitForWork, this);
	}
}

Pool::ThreadPool::~ThreadPool()
{
	tasks.stop_wait();
	Destroy();
}

inline void Pool::ThreadPool::WaitForWork()
{
	while (!stopThreads.load(std::memory_order_acquire))
	{
		tasks.wait();

		TaskPtrBase task = std::move(tasks.pop_front());
		if (task.get() != nullptr)
		{
			task->Execute();
		}

	}
}

inline void Pool::ThreadPool::Destroy()
{
	stopThreads.store(true, std::memory_order_release);
	for (auto& thread : threads)
	{
		if (thread.joinable())
		{
			thread.join();
		}
	}
}
