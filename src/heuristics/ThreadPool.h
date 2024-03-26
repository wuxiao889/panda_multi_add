#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <Heuristic.h>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

using task_type = std::function<void(Heuristic **)>;

class ThreadPool {
public:
  ThreadPool(size_t, Heuristic ***);
  ~ThreadPool();
  std::future<void> enque(task_type &);

private:
  // need to keep track of threads so we can join them
  std::vector<std::thread> workers;
  // the task queue
  std::queue<task_type> tasks;

  // synchronization
  std::mutex queue_mutex;
  std::condition_variable condition;
  bool stop;
};

// the constructor just launches some amount of workers
inline ThreadPool::ThreadPool(size_t threads, Heuristic ***hF) : stop(false) {
  for (size_t i = 0; i < threads; ++i) {
    Heuristic **h = hF[i];
    workers.emplace_back([this, h]() {
      while(!stop) {
        task_type task;
        {
          std::unique_lock<std::mutex> lock(this->queue_mutex);
          this->condition.wait(
              lock, [this] { return this->stop || !this->tasks.empty(); });
          if (this->stop && this->tasks.empty())
            return;
          task = std::move(this->tasks.front());
          this->tasks.pop();
        }
        task(h);
      }
    });
  }
}

inline std::future<void> ThreadPool::enque(task_type &task) {
  auto ts =
      std::make_shared<std::packaged_task<void(Heuristic **)>>(std::move(task));
  {
    unique_lock<std::mutex> lock(queue_mutex);
    tasks.push([ts](Heuristic **h) {
      (*ts)(h);
      return true;
    });
  }
  condition.notify_one();
  return ts->get_future();
}

// the destructor joins all threads
inline ThreadPool::~ThreadPool() {
  {
    std::unique_lock<std::mutex> lock(queue_mutex);
    stop = true;
  }
  condition.notify_all();
  for (std::thread &worker : workers)
    worker.join();
}

#endif
