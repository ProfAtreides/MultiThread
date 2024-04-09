#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <semaphore.h>
#include <random>

using namespace std;

sem_t grinderSem;
sem_t matchboxSem;

mutex consoleMutex;

void displayMessage(const string &message) {
    unique_lock<mutex> lock(consoleMutex);
    cout << message + "\n";
}

void smokerProcess(string name) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> act(50, 250);
    uniform_int_distribution<> nap(1, 5);
    while (true) {
        displayMessage(name + " is waiting for grinder");
        sem_wait(&grinderSem);
        displayMessage(name + " is whipping up a pipe");
        sem_post(&grinderSem);
        this_thread::sleep_for(chrono::milliseconds (act(gen)));
        displayMessage(name + " is waiting for matches");
        sem_wait(&matchboxSem);
        displayMessage(name + " fires up his pipe");
        sem_post(&matchboxSem);
        this_thread::sleep_for(chrono::milliseconds (act(gen)));
        displayMessage(name + " finishes smoking and lays down for a nap");
        this_thread::sleep_for(chrono::seconds(nap(gen)));
    }
}

int main() {
    int m = 1,g=1,n=5;

    sem_init(&grinderSem, 0, g);
    sem_init(&matchboxSem, 0, m);

    string name[] = {"Micheal", "Pam", "Dwight", "Jim", "Stanley", "Angela", "Kevin", "Oscar", "Creed"};

    thread *smokers = new thread[n];
    for (int i = 0; i < n; ++i) {
        smokers[i] = thread(smokerProcess, name[i]);
    }

    // Oczekiwanie na zakończenie wątków
    for (int i = 0; i < n; ++i) {
        smokers[i].join();
    }

    // Zniszczenie semaforów
    sem_destroy(&grinderSem);
    sem_destroy(&matchboxSem);

    return 0;
}
