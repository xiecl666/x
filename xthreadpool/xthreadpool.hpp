#ifndef XTHREAD_POOL_H
#define XTHREAD_POOL_H
#include<thread>
#include<queue>
#include <functional>
#include<future>
#include<condition_variable>
#include<vector>
#include<mutex>
namespace x
{
	class XthreadPool
	{
	public:
		explicit XthreadPool(int n);
		XthreadPool(const XthreadPool &)=delete;
		XthreadPool& operator=(const XthreadPool &)=delete;
		XthreadPool& operator=(const XthreadPool &&)=delete;
		XthreadPool(XthreadPool&&)=delete;
		template <class T,class... Args>
		std::future<typename std::result_of<T(Args...)>::type> submit(T&& f,Args&&... args)
			{
		typedef typename std::result_of<T(Args...)>::type return_type;
		typedef std::packaged_task<return_type()> task;
		auto t=std::make_shared<task>(std::bind(std::forward<T>(f),std::forward<Args>(args)...));
		auto res=t->get_future();
		{
			std::lock_guard<std::mutex> lg(work_queue_lk);
			work_queue_.emplace([t]{(*t)();});
		}
		work_queue_cond.notify_one();
		return res;
	}
		~XthreadPool();
	private:
		void run();
		uint32_t threadnums;
		std::vector<std::thread> threads_;
		std::queue<std::function<void()>> work_queue_;
		std::mutex work_queue_lk;
		std::condition_variable work_queue_cond;
	};


}
#endif
