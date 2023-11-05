#include <iostream>
#include <deque>
#include <future>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

mutex numbersMutex;
mutex tasksMutex;
condition_variable cv;
bool inputDone = false;

bool isPrime(int n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;

    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0)
            return false;
    }
    return true;
}

int nthPrime(int n) {
    int count = 0;
    int num = 1;
    while (count < n) {
        num++;
        if (isPrime(num)) {
            count++;
        }
    }
    return num;
}

void worker(deque<packaged_task<int()>>& tasks, deque<int>& numbers) {
    while (true) {
        unique_lock<mutex> lock(tasksMutex);
        cv.wait(lock, [&] { return !tasks.empty() || inputDone; });

        if (tasks.empty() && inputDone) {
            break;
        }

        packaged_task<int()> task(std::move(tasks.front()));
        tasks.pop_front();
        lock.unlock();

        task();
        future<int> result = task.get_future();
        lock_guard<mutex> numLock(numbersMutex);
        int number = numbers.front();
        numbers.pop_front();
        cout << "--------------------------------" << endl;
        cout << "Number: " << number << " Result: " << result.get() << endl;
        cout << "--------------------------------" << endl;
    }
}

int main() {
    deque<packaged_task<int()>> tasks;
    deque<int> numbers;

    thread workerThread(worker, ref(tasks), ref(numbers));

    int simulatedInput[] = { 7,  1000000, 100, -1 };
    const int inputDelayMs = 100;

    for (int number : simulatedInput) {
        this_thread::sleep_for(chrono::milliseconds(100));
        cout << "Enter a number (-1 to exit): " << endl;
        this_thread::sleep_for(chrono::milliseconds(inputDelayMs));
        cout << number << endl;

        if (number == -1) {
            {
                lock_guard<mutex> lock(tasksMutex);
                inputDone = true;
            }
            cv.notify_one();  // Notify worker to exit
            break;
        }

        packaged_task<int()> task([number] { return nthPrime(number); });

        {
            lock_guard<mutex> taskLock(tasksMutex);
            lock_guard<mutex> numLock(numbersMutex);
            tasks.push_back(std::move(task));
            numbers.push_back(number);
        }
        cv.notify_one(); // Notify worker of new task
    }

    workerThread.join();

    return 0;
}
