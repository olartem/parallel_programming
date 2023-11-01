#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>

using namespace std;
queue<int> dataQueue;
mutex mtx;
condition_variable cv;

void DataPreparation() {
    // Simulated user input array
    int simulatedInput[] = { 7, 11, 8, 13, -1 };
    const int inputDelayMs = 100;

    for (int i : simulatedInput) {
        //Not necessary, but it fixes the output (we get output of processing before Enter the number message)
        this_thread::sleep_for(chrono::milliseconds(100));

        cout << "Enter a number (-1 to end): " << endl;
        this_thread::sleep_for(chrono::milliseconds(inputDelayMs));
        cout << i << endl;
        {
            lock_guard<mutex> lock(mtx);
            dataQueue.push(i);
        }
        cv.notify_one();
    }
}

bool isPrime(int num) {
    if (num < 2) return false;
    for (int i = 2; i * i <= num; ++i) {
        if (num % i == 0) return false;
    }
    return true;
}

void DataProcessing() {
    while (true) {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [] { return !dataQueue.empty(); });

        int number = dataQueue.front();
        dataQueue.pop();

        if(number == -1)
            break;
        lock.unlock(); // Unlock before time-consuming prime check

        if (isPrime(number)) {
            cout << number << " is a prime number." << endl;
        }
    }
}

//Now we don't need to wait for the whole input, we process data as soon as we get it
int main() {
    thread preparationThread(DataPreparation);
    preparationThread.detach();

    thread processingThread(DataProcessing);

    processingThread.join();

    return 0;
}
