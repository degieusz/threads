#include <gtest/gtest.h>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <iostream>

#include <queue>

using namespace std;

class pool
{
	public:
		pool(int max_size_ = 10): max_size(max_size_) {};
		void push(int el)
		{
			cout << "\033[1;31m DGDG \033[0m push start\n";
			queue_mutex.lock();
			if (queue.size() < max_size) {
				cout << "queue push\n" << queue.size();
				queue.push(el);
				wait_sec(1);
			}
			else {
				queue_mutex.unlock();
				wait_sec(1);
				queue_mutex.lock();
			}
			queue_mutex.unlock();
		};
		void pop()
		{
			cout << "\033[1;31m DGDG \033[0m pop start\n";
			queue_mutex.lock();
			if (queue.size() > 0) {
				cout << "queue pop\n" << queue.size();
				queue.pop();
				wait_sec(1);
			}
			else {
				queue_mutex.unlock();
				wait_sec(1);
				queue_mutex.lock();
			}
			queue_mutex.unlock();

		}

	private:
		void wait_sec(int seconds)
		{
			cout << "wait\n";
			boost::posix_time::seconds work_time(1);
			boost::this_thread::sleep(work_time);
		}
		std::queue<int> queue;
		mutable boost::mutex queue_mutex;
		mutable boost::mutex push_mutex, pop_mutex;
		int max_size;
};

class producer
{
	public:
		producer(boost::shared_ptr<pool> storage_): storage(storage_) {};
		void produce()
		{
			while (true) {
				storage->push(1);
			}
		}
	private:
		boost::shared_ptr<pool> storage;
};

class consumer
{
	public:
		consumer(boost::shared_ptr<pool> storage_) : storage(storage_) {};
		void consume()
		{
			while (true) {
				storage->pop();
			}
		};
	private:
		boost::shared_ptr<pool> storage;
};

class market
{
	public:
		market(int producers_number_, int consumers_number_):
		  storage(new pool),
		  producers_number(producers_number_),
		  consumers_number(consumers_number_)
		{}

		void start_market()
		{
			producer prod(storage);
			consumer cons(storage);

			boost::thread prod_thread(&producer::produce, &prod);
			boost::thread cons_thread(&consumer::consume, &cons);

			cons_thread.join();
			prod_thread.join();
		};
	private:
		boost::shared_ptr<pool> storage;
		int producers_number;
		int consumers_number;
};
void worker_function()
{
	boost::posix_time::seconds work_time(3);
	std::cout << "worker running" << std::endl;
	boost::this_thread::sleep(work_time);

	std::cout << "worker finished" << std::endl;
}

TEST(test1, DISABLED_thread_spawn)
{
	cout << "start\n";
	boost::thread worker_thread(worker_function);
	cout << "wait\n";
	worker_thread.join();
	cout << "stop\n";
}
TEST(test1, producer_consumer)
{
	market market_(1, 1);
	market_.start_market();
}

