#pragma once
#include <memory>
#include <thread>
#include <future>
#include <type_traits>
#include <Utility/ConcurrentQueue.h>
#include <Core/TSingleton.h>

/* ����
Submit("�Լ�", "�Լ� �μ���", ... )
�Լ��� ȣ���ϰ� ��ȯ�� future�� ������ ���� -> �Լ��� �񵿱�� ó��
future.get(); �Լ� �Ϸ� ���

����1 : auto future = threadPool.Submit([](int a){return 1;}, 5);
int result = future.get();

����2 : void Print(const std::string& message) { std::cout << message << std::endl; }
auto future = threadPool.Submit(Print, "������~");
future.get();

*/
class ThreadPool;
extern ThreadPool& threadPool;

class ThreadPool : public TSingleton<ThreadPool>
{
	friend class TSingleton<ThreadPool>;

private:
	ThreadPool() = default;

public:
	ThreadPool(ThreadPool const&) = delete;
	ThreadPool(ThreadPool&&) = delete;
	ThreadPool& operator=(ThreadPool const&) = delete;
	ThreadPool& operator=(ThreadPool&&) = delete;
	
public:
	~ThreadPool() = default;

public:
	void Initialize(unsigned int pool_size = std::thread::hardware_concurrency() - 1)
	{
		done = false;

		static const unsigned int maxThreads = std::thread::hardware_concurrency();
		const unsigned int numThreads = pool_size == 0 ? maxThreads - 1 : (std::min)(maxThreads - 1, pool_size);

		threads.reserve(numThreads);
		for (unsigned int i = 0; i < numThreads; ++i)
		{
			threads.emplace_back(std::bind(&ThreadPool::ThreadWork, this));
		}
	}

	void Destroy()
	{
		if (done) return;
		{
			std::unique_lock<std::mutex> lk(condMutex);
			done = true;
			condVar.notify_all();
		}

		for (unsigned int i = 0; i < threads.size(); ++i) if (threads[i].joinable())  threads[i].join();
	}

	template<typename F, typename... Args>
	auto Submit(F&& f, Args&&... args)
	{
		using ReturnType = std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>;

		auto bindFunc = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
		auto wrappedTask = std::make_shared<std::packaged_task<ReturnType()>>(bindFunc);
		std::future<ReturnType> resultFuture = wrappedTask->get_future();
		auto voidTask = [wrappedTask]() {(*wrappedTask)(); };
		taskQueue.Push(std::move(voidTask));
		condVar.notify_one();

		return resultFuture;
	}

private:
	void ThreadWork()
	{
		std::function <void()> task;
		bool popSuccess;

		while (true)
		{
			{
				std::unique_lock<std::mutex> lk(condMutex);
				while (!done && taskQueue.Empty()) condVar.wait(lk);
				if (done) return;
				else popSuccess = taskQueue.TryPop(task);
			}

			if (popSuccess)
			{
				task();
				task = nullptr;
			}
		}
	}

private:
	std::vector<std::thread> threads;
	Utility::ConcurrentQueue<std::function<void()>> taskQueue;
	bool done = false;
	std::condition_variable condVar;
	std::mutex condMutex;
	
};