#include "xthreadpool.hpp"
namespace x
{
	XthreadPool::XthreadPool(int n=0)
	{
		threadnums=n;
		if(threadnums<=0)
		{
			threadnums=std::thread::hardware_concurrency();
			threadnums=(threadnums==0?2:threadnums);
		}
		for(int i=0;i<threadnums;i++)
		{
			threads_.push_back(std::thread([this](){ this->run(); }));
		}
	}
	XthreadPool::~XthreadPool()
	{
		for(auto& i:threads_)
		{
			if(i.joinable())
			{
				i.join();
			}
		}
	}
//	template <class T,class... Args>
//		std::future<typename std::result_of<T(Args...)>::type> XthreadPool::submit(T&& f,Args&&... args)
//	{
//		typedef typename std::result_of<T(Args...)>::type return_type;
//		typedef std::packaged_task<return_type()> task;
//		auto t=std::make_shared<task>(std::bind(std::forward<T>(f),std::forward<Args>(args)...));
//		auto res=t->get_future();
//		{
//			std::lock_guard<std::mutex> lg;
//			work_queue_.emplace([t]{(*t)();});
//		}
//		work_queue_cond.notify_one();
//		return res;
//	}
	void XthreadPool::run()
	{
		while(1)
		{
			std::function<void()> task;
			{
			std::unique_lock<std::mutex> ulk(work_queue_lk);
			work_queue_cond.wait(ulk,[this]{return !this->work_queue_.empty();});
			task=std::move(work_queue_.front());
			work_queue_.pop();
			}
			task();
		}
	}
}
