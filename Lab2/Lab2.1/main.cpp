#include <iostream>
#include <thread>
#include <mutex>
#include <list>
#include <algorithm>

using namespace std;

list<int> l;
list<string> foundList;
mutex mtx;

void AddToList(int value) {
    lock_guard<mutex> lock(mtx);
    l.push_back(value);
}

void ListContains(int target) {
    lock_guard<mutex> lock(mtx);
    bool found = (find(l.begin(), l.end(), target) != l.end());
    if (found) {
        foundList.push_back("Found: " + to_string(target));
    } else {
        foundList.push_back("Not Found: " + to_string(target));
    }
}

int main() {
    for (int i = 1; i <= 10; ++i) {
        thread addThread(AddToList, i);
        addThread.detach();

        thread checkThread(ListContains, i);
        checkThread.detach();
    }
    for (const string& result : foundList) {
        cout << result << endl;
    }
    return 0;
}