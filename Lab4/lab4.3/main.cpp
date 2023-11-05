#include <iostream>
#include <cmath>
#include <thread>
#include <future>

using namespace std;

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

void firstThread(promise<int> &p1, promise<int> &p2, promise<bool> &signal, int n) {
    int count = 0;
    int nthPrime = 2, nTimes10Prime = 2;

    while (count < n) {
        if (isPrime(nthPrime)) {
            count++;
        }
        if (count == n) {
            //Send nth prime and signal for second thread
            p1.set_value(nthPrime);
            signal.set_value(true);
        }
        else {
            nthPrime++;
        }
    }

    count = 0;
    while (count < n * 10) {
        if (isPrime(nTimes10Prime)) {
            count++;
        }
        nTimes10Prime++;
    }
    //Send 10nth prime
    p2.set_value(nTimes10Prime - 1);
}

void secondThread(future<bool> &signal, int n) {
    signal.wait(); // Wait for signal from the first thread
    bool flag = signal.get();

    if (flag) {
        this_thread::sleep_for(chrono::seconds(2));
        double sqrtN = sqrt(static_cast<double>(n));
        cout << "Square root of n is: " << sqrtN << endl;
    }
}

int main() {
    int n;
    cout << "Enter the value of n to find the nth prime number: " << endl;
    this_thread::sleep_for(chrono::seconds(1));
    n = 100000;
    cout << n << endl;

    promise<int> NthPrimePromise, NTimes10PrimePromise;
    promise<bool> signal;
    future<bool> signalFuture = signal.get_future();
    future<int> NthPrimeFuture = NthPrimePromise.get_future();
    future<int> NTimes10PrimeFuture = NTimes10PrimePromise.get_future();

    thread t1(firstThread, ref(NthPrimePromise), ref(NTimes10PrimePromise),ref(signal), n);
    thread t2(secondThread, ref(signalFuture), n);

    int nthPrime = NthPrimeFuture.get();
    cout << "The nth prime number is: " << nthPrime << endl;

    int nTimes10Prime = NTimes10PrimeFuture.get();
    cout << "The 10 x nth prime number is: " << nTimes10Prime << endl;

    t1.join();
    t2.join();

    return 0;
}
