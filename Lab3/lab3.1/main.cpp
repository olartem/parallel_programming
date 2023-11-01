#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <chrono>

using namespace std;
queue<int> dataQueue;
mutex mtx;
bool dataReady = false;

void DataPreparation() {
    // Simulated user input array
    int simulatedInput[] = { 7, 11, 8, 13, -1 };
    const int inputDelayMs = 100;

    for (int i : simulatedInput) {
        cout << "Enter a number (-1 to end): " << endl;
        this_thread::sleep_for(chrono::milliseconds(inputDelayMs));
        cout << i << endl;
        {
            lock_guard<mutex> lock(mtx);
            if (i == -1) {
                dataReady = true;
                break;
            } else {
                dataQueue.push(i);
            }
        }
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
    while (!dataReady) {
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    cout << "Started processing" << endl;
    while (!dataQueue.empty()) {
        int number;

        mtx.lock();
        number = dataQueue.front();
        dataQueue.pop();
        mtx.unlock();

        if (isPrime(number)) {
            cout << number << " is a prime number." << endl;
        }
    }
}

//The result in invariant, but we wait for the whole input before processing
int main() {
    thread preparationThread(DataPreparation);
    preparationThread.detach();

    thread processingThread(DataProcessing);

    processingThread.join();

    return 0;
}
