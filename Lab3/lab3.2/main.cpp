#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;
mutex mtx;
condition_variable cv;
int globalVar = 0;

void Waits(int id) {
    unique_lock<mutex> lock(mtx);
    cout << "Thread " << id << " entering the wait state." << endl;
    cv.wait(lock, [] { return globalVar == 1; });
    cout << "Thread " << id << " wait is over." << endl;
}

void Awake() {
    this_thread::sleep_for(chrono::milliseconds(100));
    cout << "Awake thread starting to notify the rest." << endl;

    {
        lock_guard<mutex> lock(mtx);
        cv.notify_all();
        cout << "First notification sent." << endl;
    }

    this_thread::sleep_for(chrono::milliseconds(100));
    {
        lock_guard<mutex> lock(mtx);
        globalVar = 1;
        cv.notify_all();
        cout << "Second notification sent after variable assignment." << endl;
    }
}

int main() {
    thread t1(Waits, 1);
    thread t2(Waits, 2);
    thread t3(Waits, 3);
    thread tAwake(Awake);

    tAwake.join();
    // The order of threads obtaining the mutex is not invariant
    t1.join();
    t2.join();
    t3.join();

    return 0;
}
