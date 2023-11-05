#include <iostream>
#include <future>
#include <cmath>
#include <chrono>
#include <thread>

using namespace std;
bool isPrime(int num) {
    if (num <= 1) return false;
    for (int i = 2; i * i <= num; ++i) {
        if (num % i == 0) return false;
    }
    return true;
}

int nthPrime(int n) {
    int count = 0;
    int num = 2;

    while (count < n) {
        if (isPrime(num)) {
            count++;
        }
        num++;
    }
    return num - 1;
}

double async(int n)
{
    auto startAsync = chrono::high_resolution_clock::now();
    future<int> resultAsync = async(launch::async, nthPrime, n);

    cout << "While we calculate, did you know:" << endl;

    cout << "Square root of " << n << " is approximately: " << sqrt(n) << endl;
    cout << "Sine of " << n << " is approximately: " << sin(n) << endl;
    cout << "Natural logarithm of " << n << " is approximately: " << log(n) << endl;

    int primeNumber = resultAsync.get();
    auto endAsync = chrono::high_resolution_clock::now();
    chrono::duration<double> timeDeferred = endAsync - startAsync;
    cout << "The " << n << "th prime number is: " << primeNumber << endl;
    return timeDeferred.count();
}

double deferred(int n)
{
    auto startDeferred = chrono::high_resolution_clock::now();
    future<int> resultDeferred = async(launch::deferred, nthPrime, n);

    cout << "While we calculate, did you know:" << endl;

    cout << "Square root of " << n << " is approximately: " << sqrt(n) << endl;
    cout << "Sine of " << n << " is approximately: " << sin(n) << endl;
    cout << "Natural logarithm of " << n << " is approximately: " << log(n) << endl;

    int primeNumber = resultDeferred.get();
    auto endDeferred = chrono::high_resolution_clock::now();
    chrono::duration<double> timeDeferred = endDeferred - startDeferred;
    cout << "The " << n << "th prime number is: " << primeNumber << endl;
    return timeDeferred.count();
}

int main() {
    int n;
    cout << "Enter the value of n to find the nth prime number: " << endl;
    this_thread::sleep_for(chrono::seconds(1));
    n = 1000000;
    cout << n << endl;

    cout << "ASYNC" << endl;
    double asyncTime = async(n);
    cout << endl << endl;

    cout << "DEFERRED" << endl;
    double deferredTime = deferred(n);
    cout << endl << endl;

    //Deferred time is slightly higher due to deferred start(0.05 - 0.1 higher comparing to async calculating 10^6th prime)
    cout << "Time taken to compute using launch::async: " << asyncTime << " seconds" << endl;
    cout << "Time taken to compute using launch::deferred: " << deferredTime << " seconds" << endl;

    return 0;
}
