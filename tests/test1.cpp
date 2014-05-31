#include <gtest/gtest.h>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include <boost/thread/thread.hpp>
#include <boost/thread/condition_variable.hpp>
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
			if (queue.size() < max_size) {
				boost::mutex::scoped_lock lock(mtx);
				cout << "queue push\n" << queue.size();
				queue.push(el);
				wait_sec(1);
				cond_queue.notify_one();
			}
			else {
				wait_sec(1);
			}
		};
		void pop()
		{
			cout << "\033[1;31m DGDG \033[0m pop start\n";
			boost::mutex::scoped_lock lock(mtx);
			if (queue.size() > 0) {
				cout << "queue pop\n" << queue.size();
				queue.pop();
				wait_sec(1);
			}
			else {
				cond_queue.wait(lock);
				wait_sec(1);
			}
		}

	private:
		void wait_sec(int seconds)
		{
			cout << "wait\n";
			boost::posix_time::seconds work_time(1);
			boost::this_thread::sleep(work_time);
		}
		std::queue<int> queue;
		mutable boost::mutex mtx;
		mutable boost::condition_variable cond_queue;
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

TEST(test1, producer_consumer)
{
	market market_(1, 1);
	market_.start_market();
}

