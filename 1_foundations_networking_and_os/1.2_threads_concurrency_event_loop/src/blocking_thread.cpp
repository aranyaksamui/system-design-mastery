#include <iostream>
#include <thread>
#include <chrono>
#include <vector>

// Simulating a network or database request that takes 2 seconds (Blocking I/O)
void handleRequestBlocking(int requestId)
{
    std::cout << "[Thread " << std::this_thread::get_id() << "] Request " << requestId << " received. Blocking thread for 2 seconds..." << std::endl;

    // The OS puts this specific thread to sleep, freezing it's execution entirely
    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::cout << "[Thread " << std::this_thread::get_id() << "] Request " << requestId << " completed." << std::endl;
}

int main()
{
    std::cout << "Starting blocking thread simulation..." << std::endl;

    // A vector of threads
    std::vector<std::thread> threadPool;

    // Create 3 threads and insert them in threadPool vector
    for (int i = 0; i < 3; i++)
    {
        // We must ask the os to spawn a brand new thread for each request to avoid freezing the main application
        threadPool.push_back(std::thread(handleRequestBlocking, i));
    }

    // Pause the main thread to finish the tasks of worker threads in concurrency
    for (auto& t : threadPool) t.join();

    return 0;
}