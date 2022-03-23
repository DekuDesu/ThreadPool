#include <iostream>
#include "ConcurrentQueue.h"
#include "ThreadPool.h"
#include "ThreadPool.cpp"
#include "ConcurrentQueue.cpp"

std::mutex _coutLocker;

class Work : public WorkItem {
private:
    std::string word;

public:
    Work(std::string word) : word(word) {}
    // Inherited via WorkItem
    bool PerformWork(SentinelToken& token) override {

        // pretty sure cout isn't thread safe
        // the cppref wasn't clear
        std::unique_lock<std::mutex> lock(_coutLocker);
        std::cout << word;
        return true;
    }
};

int main() {
    ThreadPool<Work> pool(4);

    for (size_t i = 0; i < 4; i++) {
        pool.EnqueueWork(Work(std::to_string(i) + '\n'));
    }

    pool.PerformWork();

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    pool.CancelWork();

    pool.Wait();

    return 0;
}