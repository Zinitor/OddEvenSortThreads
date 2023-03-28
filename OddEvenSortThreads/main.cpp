#include <iostream>
#include <thread>
#include <algorithm>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>
#include <barrier>

// Define global variables
const int ARRAY_SIZE = 100;
const int THREAD_COUNT = 2;
const int MAX_RANDOM = 100;
int data[ARRAY_SIZE];

// Define a mutex and condition variable to coordinate threads
std::mutex mtx;
std::condition_variable cv;
bool is_sorted = false;

// Define a barrier to synchronize threads
std::barrier<THREAD_COUNT> barrier(THREAD_COUNT);

// Define a function to print the contents of the array
void print_array() {
    std::cout << "Sorted array: ";
    for (int i = 0; i < ARRAY_SIZE; i++) {
        std::cout << data[i] << " ";
    }
    std::cout << std::endl;
}

// Define a function to perform odd-even sort on the array
void odd_even_sort(int tid) {
    int start, end;
    if (tid == 0) {
        start = 1;
        end = ARRAY_SIZE - 1;
    }
    else {
        start = 0;
        end = ARRAY_SIZE - 2;
    }
    for (int i = start; i <= end; i += 2) {
        if (data[i] > data[i + 1]) {
            std::swap(data[i], data[i + 1]);
        }
    }
    // Wait for all threads to finish before proceeding
    barrier.wait();
    // If this is the last iteration and the array is sorted, notify other threads to exit
    if (tid == 0 && is_sorted) {
        cv.notify_all();
    }
    for (int i = start; i <= end; i += 2) {
        if (data[i] > data[i + 1]) {
            std::swap(data[i], data[i + 1]);
            is_sorted = false;
        }
    }
    // Wait for all threads to finish before proceeding
    barrier.wait();
}

int main() {
    // Generate random numbers and print unsorted array
    std::srand(std::time(nullptr));
    std::cout << "Unsorted array: ";
    for (int i = 0; i < ARRAY_SIZE; i++) {
        data[i] = std::rand() % MAX_RANDOM;
        std::cout << data[i] << " ";
    }
    std::cout << std::endl;
    // Start the sorting threads
    std::vector<std::thread> threads;
    for (int i = 0; i < THREAD_COUNT; i++) {
        threads.emplace_back(odd_even_sort, i);
    }
    // Wait for all threads to finish
    for (auto& t : threads) {
        t.join();
    }
    // Print the sorted array
    print_array();
    return 0;
}
