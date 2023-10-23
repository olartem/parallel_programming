#include <iostream>
#include <string>
#include <thread>
#include <mutex>
using namespace std;
class someData {
public:
    string firstname;
    string lastname;
    string address;
    int age;

    someData(const string& first, const string& last, const string& addr, int a)
            : firstname(first), lastname(last), address(addr), age(a) {}
};

class exchangePerson {
public:
    someData data;
    mutex mtx;

    exchangePerson(const someData& initialData) : data(initialData) {}

    static void JohnDoe(exchangePerson& person) {
        lock_guard<mutex> lock(person.mtx);
        cout << "Person Data Before JohnDoe: " << person.data.firstname << " " << person.data.lastname << " "
             << person.data.address << " " << person.data.age << endl;
        person.data.firstname = "John";
        person.data.lastname = "Doe";
        person.data.address = "Unknown";
        person.data.age = 120;
        cout << "Person Data After JohnDoe: " << person.data.firstname << " " << person.data.lastname << " "
             << person.data.address << " " << person.data.age << endl;
    }

    static void JacobSmith(exchangePerson& person) {
        lock_guard<mutex> lock(person.mtx);
        cout << "Person Data Before JacobSmith: " << person.data.firstname << " " << person.data.lastname << " "
             << person.data.address << " " << person.data.age << endl;
        person.data.firstname = "Jacob";
        person.data.lastname = "Smith";
        person.data.address = "Known";
        person.data.age = 1;
        cout << "Person Data After JacobSmith: " << person.data.firstname << " " << person.data.lastname << " "
             << person.data.address << " " << person.data.age << endl;
    }

    static void AdoptSwap(exchangePerson& person1, exchangePerson& person2) {
        // lock_guard can't get 2 locks simultaneously, so we use adopt_lock to avoid deadlock
        // adopt_lock assumes that the calling thread already owns the lock
        // Ownership will be released on destructor call
        if(&person1 == &person2)
        {
            cout << "Same Reference" << endl;
            return;
        }
        lock(person1.mtx,person2.mtx);
        lock_guard<mutex> lock1(person1.mtx, adopt_lock);
        lock_guard<mutex> lock2(person2.mtx, adopt_lock);
        cout << "Person 1 Data Before Adopt Swap: " << person1.data.firstname << " " << person1.data.lastname << " "
             << person1.data.address << " " << person1.data.age << endl;
        cout << "Person 2 Data Before Adopt Swap: " << person2.data.firstname << " " << person2.data.lastname << " "
             << person2.data.address << " " << person2.data.age << endl;

        swap(person1.data, person2.data);

        cout << "Person 1 Data After Adopt Swap: " << person1.data.firstname << " " << person1.data.lastname << " "
             << person1.data.address << " " << person1.data.age << endl;
        cout << "Person 2 Data After Adopt Swap: " << person2.data.firstname << " " << person2.data.lastname << " "
             << person2.data.address << " " << person2.data.age << endl;
    }

    static void DeferSwap(exchangePerson& person1, exchangePerson& person2) {
        // unique_lock may delay ownership of mutex using defer_lock and
        // assumes that the calling thread is going to call lock to acquire the mutex
        // Ownership will be released on destructor call
        if(&person1 == &person2)
        {
            cout << "Same Reference" << endl;
            return;
        }
        unique_lock<mutex> lock1(person1.mtx, defer_lock);
        unique_lock<mutex> lock2(person2.mtx, defer_lock);
        lock(lock1,lock2);
        cout << "Person 1 Data Before Defer Swap: " << person1.data.firstname << " " << person1.data.lastname << " "
             << person1.data.address << " " << person1.data.age << endl;
        cout << "Person 2 Data Before Defer Swap: " << person2.data.firstname << " " << person2.data.lastname << " "
             << person2.data.address << " " << person2.data.age << endl;

        swap(person1.data, person2.data);

        cout << "Person 1 Data After Defer Swap: " << person1.data.firstname << " " << person1.data.lastname << " "
             << person1.data.address << " " << person1.data.age << endl;
        cout << "Person 2 Data After Defer Swap: " << person2.data.firstname << " " << person2.data.lastname << " "
             << person2.data.address << " " << person2.data.age << endl;
    }
};

int main() {
    someData initialData1("Alice", "Johnson", "123 Main St", 30);
    someData initialData2("Bob", "Smith", "456 Elm St", 25);

    exchangePerson person1(initialData1);
    exchangePerson person2(initialData2);

    thread thread1(&exchangePerson::JacobSmith, ref(person1));
    thread1.detach();
    thread thread2(&exchangePerson::JohnDoe, ref(person1));
    thread2.detach();
    thread thread3(&exchangePerson::JohnDoe, ref(person2));
    thread3.detach();
    thread thread4(&exchangePerson::JacobSmith, ref(person2));
    thread4.detach();
    thread thread5(&exchangePerson::AdoptSwap, ref(person1), ref(person2));
    thread5.detach();
    thread thread6(&exchangePerson::DeferSwap, ref(person1), ref(person2));
    thread6.detach();

    this_thread::sleep_for(chrono::seconds(1));
    lock_guard<mutex> lock1(person1.mtx);
    lock_guard<mutex> lock2(person2.mtx);

    // Output the updated data after all threads have finished
    cout << "Person 1 Data: " << person1.data.firstname << " " << person1.data.lastname << " "
              << person1.data.address << " " << person1.data.age << endl;
    cout << "Person 2 Data: " << person2.data.firstname << " " << person2.data.lastname << " "
              << person2.data.address << " " << person2.data.age << endl;

    return 0;
}
