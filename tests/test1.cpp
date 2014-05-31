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
			cout << boost::this_thread::get_id() << " :";
			//boost::mutex::scoped_lock lock(mtx);
			if (queue.size() < max_size) {
				//boost::mutex::scoped_lock lock(mtx);
				queue.push(el);
				cout << "push" << queue.size() << endl;
				wait_sec(1);
				to_few.notify_one();
			}
			else {
				wait_sec(1);
				//to_much.wait(lock);
				cout << "\033[1;31m DGDG \033[0m cant push\n";
			}
		};
		void pop()
		{
			cout << boost::this_thread::get_id() << " :";
			boost::mutex::scoped_lock lock(mtx);
			if (queue.size() > 0) {
				cout << "pop" << queue.size() << endl;
				queue.pop();
				//to_much.notify_one();
				wait_sec(1);
			}
			else {
				cout << "\033[1;31m DGDG \033[0m cant pop\n";
				to_few.wait(lock);
				wait_sec(1);
			}
		}

	private:
		void wait_sec(int seconds)
		{
			boost::posix_time::seconds work_time(1);
			boost::this_thread::sleep(work_time);
		}
		std::queue<int> queue;
		mutable boost::mutex mtx;
		mutable boost::condition_variable to_few, to_much;
		int max_size;
};

class producer
{
	public:
		producer(boost::shared_ptr<pool> storage_): storage(storage_) {my_id = ++number;};
		void produce()
		{
			while (true) {
				//cout << "\033[1;31m DGDG \033[0m producer" << my_id << endl;
				storage->push(my_id);
			}
		}
	private:
		boost::shared_ptr<pool> storage;
		static int number;
		int my_id;
};

class consumer
{
	public:
		consumer(boost::shared_ptr<pool> storage_) : storage(storage_) {my_id = number;};
		void consume()
		{
			while (true) {
				//cout << "\033[1;31m DGDG \033[0m consument" << my_id << endl;
				storage->pop();
			}
		};
	private:
		boost::shared_ptr<pool> storage;
		static int number;
		int my_id;
};
int consumer::number = 0;
int producer::number = 0;
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
			//producer prod2(storage);
			consumer cons(storage);
			consumer cons2(storage);

			boost::thread prod_thread(&producer::produce, &prod);
			//boost::thread prod2_thread(&producer::produce, &prod2);
			boost::thread cons_thread(&consumer::consume, &cons);
			boost::thread cons2_thread(&consumer::consume, &cons2);

			cons_thread.join();
			cons2_thread.join();
			prod_thread.join();
			//prod2_thread.join();
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

