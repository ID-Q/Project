/*************************************************************************
	> File Name: threadpool.h
	> Author: 
	> Mail: 
	> Created Time: 三 10/ 7 19:00:49 2020
 ************************************************************************/

#ifndef _THREADPOOL_H
#define _THREADPOOL_H

#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace pool {
class Task {
public :
    template<typename Func_T, typename ...ARGS>
    Task(Func_T f, ARGS ...args) {
        _func = std::bind(f, std::forward<ARGS>(args)...);
    }
    void run() {
        _func();
    }
private:
    std::function<void()> _func;
};

class ThreadPool {
public:
    ThreadPool(int thread_size = 5)
    : _thread_size(thread_size),
      _started(false),
      _mutex(),
      _mutex2(),
      _cond(),
      _queue_cond(),
      _queue_mutex()
    {}
    void start();
    void stop();
    template<typename Func_T, typename ...ARGS>
    void add_one_task(Func_T f, ARGS...args) {
        std::unique_lock<std::mutex> lock(_queue_mutex);
        add_one_task(new Task(f, std::forward<ARGS>(args)...));
    }
    void stop_until_empty();
    ~ThreadPool() { stop(); }
private:
    void thread_loop();
    Task *get_one_task();
    void add_one_task(Task *);
    void stop_set_false();

    int _thread_size;
    volatile bool _started;
    std::queue<Task *> _tasks;
    std::mutex _mutex, _mutex2;
    std::mutex _queue_mutex;
    std::condition_variable _cond;
    std::condition_variable _queue_cond;
    std::vector<std::thread *> _threads;
};

void ThreadPool::start() {
    std::unique_lock<std::mutex> lock(_mutex);
    _started = true;
    for (int i = 0; i < _thread_size; i++) {
        _threads.push_back(new std::thread(&ThreadPool::thread_loop, this));
    }
}

void ThreadPool::stop() {
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _started = false;
        _cond.notify_all();
    }
    for (int i = 0; i < _threads.size(); i++) {
        _threads[i]->join();
        delete _threads[i];
    }
    _threads.clear();
    return ;
}

void ThreadPool::thread_loop() {
    while (_started) {
        Task *t = get_one_task();
        if (t != nullptr) {
            t->run();
        }
    }
    return ;
}

void ThreadPool::add_one_task(Task *t) {
    std::unique_lock<std::mutex> lock(_mutex);
    _tasks.push(t);
    _cond.notify_one();
    return ;
}

Task* ThreadPool::get_one_task() {
    std::unique_lock<std::mutex> lock(_mutex);
    while (_tasks.empty() && _started) {
        _cond.wait(lock);
    }
    Task *t = nullptr;
    if (!_tasks.empty() && _started) {
        t = _tasks.front();
        _tasks.pop();
        if (_tasks.empty()) {
            std::unique_lock<std::mutex> lock2(_mutex2);
            _queue_cond.notify_all();
        }
    }
    return t;
}

void ThreadPool::stop_until_empty() {
    std::unique_lock<std::mutex> lock1(_mutex2);
    std::unique_lock<std::mutex> lock2(_queue_mutex);
    while (!_tasks.empty()) {
        _queue_cond.wait(lock1);
    }
    stop();
    return ;
}
}

#endif
