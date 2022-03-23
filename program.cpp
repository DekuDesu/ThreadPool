#include <iostream>
#include "ConcurrentQueue.h"
#include "ThreadPool.h"
#include "ThreadPool.cpp"
#include "ConcurrentQueue.cpp"
#include <string>

std::mutex _coutLocker;

std::atomic<size_t> counter;

class Work : public WorkItem {
private:
    std::string word;

public:
    Work(std::string word) : word(word) {}
    // Inherited via WorkItem
    bool PerformWork(SentinelToken& token) override {

        //// pretty sure cout isn't thread safe
        //// the cppref wasn't clear
        std::unique_lock<std::mutex> lock(_coutLocker);
        std::cout << counter.fetch_add(1) << '\n';
        return true;
    }
};

int main() {
    ThreadPool<Work> pool(4);

    for (size_t i = 0; i < 1000000; i++) {
        pool.EnqueueWork(Work(std::to_string(i) + '\n'));
    }

    pool.PerformWork();

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    pool.CancelWork();

    pool.Wait();

    std::cout << "Final Value: " << counter.load();

    return 0;
}