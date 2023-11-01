#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;
mutex mtx;
condition_variable cv;
int globalVar = 0;

void ThreadFunc(int id) {
    unique_lock<mutex> lock(mtx);
    cout << "Thread " << id << " entering the wait state." << endl;
    cv.wait(lock, [] { return globalVar == 1; });
    cout << "Message from thread " << id << endl;
}

void Notify() {
    this_thread::sleep_for(chrono::milliseconds(100));

    {
        lock_guard<mutex> lock(mtx);
        globalVar = 1;
        cv.notify_one();
    }
    cout << "Notify finished." << endl;
}

int main() {
    thread Thread1(ThreadFunc, 1);
    thread Thread2(ThreadFunc, 2);
    thread Thread3(ThreadFunc, 3);
    thread NotifyThread(Notify);
    // Only one thread gets the mutex(program never finishes)
    NotifyThread.join();
    Thread1.join();
    Thread2.join();
    Thread3.join();

    return 0;
}
