#include <iostream>
#include <thread>
#include <mutex>
#include <list>
#include <algorithm>

using namespace std;

void task1()
{
    //Error: terminate called without an active exception
    //Thread object is out of scope while it's in joinable state
    thread t1([](){cout << 1 << endl;});
    thread t2([](){cout << 2 << endl;});
    this_thread::sleep_for(chrono::milliseconds(10));
}

void task2()
{
    //No errors, but result isn't invariant(we don't know exactly which number is printed first)
    thread t1([](){cout << 1 << endl;});
    thread t2([](){cout << 2 << endl;});
    this_thread::sleep_for(chrono::milliseconds(10));
    t1.detach();
    t2.detach();
}

list<int> l;
mutex mtx;

void AddToList(int value) {
    for (int i = 0; i < 10; ++i) {
#ifdef INVARIANT
        lock_guard<mutex> lock(mtx);
#endif
        l.push_back(value);
        cout << "Added: " << value << std::endl;
        ++value;
    }
}

void ListContains(int target) {
    for (int i = 0; i < 10; ++i) {
#ifdef INVARIANT
        lock_guard<mutex> lock(mtx);
#endif
        bool found = (find(l.begin(), l.end(), target) != l.end());
        if (found) {
            cout << "Found: " << target << std::endl;
        } else {
            cout << "Not Found: " << target << std::endl;
        }
        target++;
    }
}

void taskList(int value)
{
    std::thread addThread(AddToList, value);
    std::thread checkThread(ListContains, value);

    addThread.join();
    checkThread.join();
}

int main() {
    //task1();
    task2();
    taskList(1);
    //Remove comment for invariance(when we use runtime if statement it still may not have invariant result)
    //#define INVARIANT
    return 0;
}