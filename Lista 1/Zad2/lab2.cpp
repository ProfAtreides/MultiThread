#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <random>
#include <vector>
#include <ctime>

using namespace std;

int dataWritten = 0;
atomic<bool> canBePickedUp;
atomic<int> readCount;

mutex mtx;

condition_variable cv_canBeWritten, cv_canBeRead;

void writer(int index) {

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(50, 500);
    uniform_int_distribution<> nap(1, 3);

    while (true) {
        unique_lock<mutex> lock(mtx);
        int addPages = dis(gen);

        if(dataWritten == 0){
        }
        else
        {
            cv_canBeWritten.wait(lock, [] { return readCount >= 3; });

        }

        dataWritten += addPages;
        cout << "Writer " << index << " wrote: " << addPages << endl;
        cout <<std::flush;

        readCount = 0;
        cv_canBeRead.notify_all();

        this_thread::sleep_for(chrono::seconds(nap(gen)));
    }
}

void reader(int id) {
    while (true) {
        {
            unique_lock<mutex> lock(mtx);
            cv_canBeRead.wait(lock, [] { return dataWritten > 0; });
            readCount++;

            cout << "Reader " << id << " read: " << dataWritten << endl;
            cout <<std::flush;

            if (readCount == 3) {
                cv_canBeWritten.notify_one();
            }
        }
        this_thread::sleep_for(chrono::milliseconds(500));
    }
}

int main() {
    int numReaders = 5;
    int numWriters = 2;

    thread *readers = new thread[numReaders];
    thread *writers = new thread[numWriters];

    for (int i = 0; i < numReaders; i++) {
        readers[i] = std::move(thread(reader, i));
    }

    for (int i = 0; i < numWriters; i++) {
        writers[i] = std::move(thread(writer, i));
    }

    for (int i = 0; i < numReaders; i++) {
        readers[i].join();
    }

    for (int i = 0; i < numWriters; i++) {
        writers[i].join();
    }
}