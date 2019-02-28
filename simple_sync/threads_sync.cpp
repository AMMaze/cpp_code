#include <iostream>
#include <thread>
#include <queue>
#include <mutex>

int main() {
    size_t count = 0;
    bool done = false;
    std::queue<int> items;
    std::mutex m;
    std::thread producer([&]() {
        for (int i = 0; i < 10000; ++i) {
            std::lock_guard<std::mutex> lock(m);
            items.push(i);
            count++;
        }
        std::lock_guard<std::mutex> lock(m);
        done = true;
    });

    std::thread consumer([&]() {
        while (!done) {
            std::lock_guard<std::mutex> lock(m);
            while (!items.empty()) {
                items.pop();
                count--;
            }
        }
    });

    producer.join();
    consumer.join();
    std::cout << count << std::endl;
}
