#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <cstdlib>
#include <utility>
#include <mutex>
#include <windows.h>

using namespace std;

atomic_bool free_forks[2000] = {true, true, true, true, true, true, true, true, true, true};
mutex forks[2000];

struct philo {
    philo() {
        status = "ded";
        time = -1;
    }

    philo(string status, int index, int leftForkIndex, int rightForkIndex) :
            status(std::move(status)),
            index(index),
            time(0),
            left_fork_index(leftForkIndex),
            right_fork_index(rightForkIndex) {
    }

    string status;
    int index;
    int time;
    int left_fork_index;
    int right_fork_index;

    void dine(mutex &left, mutex &right) {

        cout << "Philosopher just sat down\n";

        while (true) {
            int duration = rand() % 1000 + 500;
            time = duration;
            status = "thinking";
            this_thread::sleep_for(chrono::milliseconds(duration));
            status = "hungry";
            time = duration;
            duration = 5000;
            while(!free_forks[left_fork_index] && !free_forks[right_fork_index])
            {
            }
            left.lock();
            right.lock();
            free_forks[left_fork_index] = false;
            free_forks[right_fork_index] = false;
            duration = rand() % 1000 + 500;
            status = "eating";
            time = duration;
            this_thread::sleep_for(chrono::milliseconds(duration));
            left.unlock();
            right.unlock();
            free_forks[left_fork_index] = true;
            free_forks[right_fork_index] = true;
        }
    }

    void manageTime(int time)
    {
        switch (status[0]) {
            case 't':
                this->time-=time;
                break;
            case 'h':
                this->time += time;
                break;
            case 'e':
                this->time -= time;
                break;
        }

    }
};

string number_to_string(int number) {
    string parsed = "";
    while (number > 0) {
        parsed += '0' + number % 10;
        number /= 10;
    }
    string reversed = "";
    for (int i = parsed.length() - 1; i >= 0; i--) {
        reversed += parsed[i];
    }
    return reversed;
}

void dinner_status(philo *guests, int n) {
    int time = 1000;
    while (true) {
        this_thread::sleep_for(chrono::milliseconds(time/2));
        system("cls");
        for (int i = 0; i < n; i++) {
            guests[i].manageTime(time);
            cout << "Philosopher " << guests[i].index << " is " << guests[i].status << " for " << guests[i].time
                 << " milliseconds." << endl;
        }
        cout << "Fork availability: ";
        for (int i = 0; i < n * 2; i += 2) {
            cout << (free_forks[i] ? "1" : "0") << " ";
        }
        cout << endl;
        this_thread::sleep_for(chrono::milliseconds(time/2));
        system("cls");

    }
}

int main() {
    int n = 5;

    philo guests[5];
    thread t[5];

    for (int i = 0; i < n; i++) {
        int right_index = (i + 1 < n) ? i : 0;
        int left_index = (i + 1 < n) ? i + 1 : i;
        guests[i] = philo("ready for a feast", i + 1, left_index, right_index);
        t[i] = thread([i, &guests] {
            guests[i].dine(forks[guests[i].left_fork_index], forks[guests[i].right_fork_index]);
        });
    }

    thread status([&, n] {
        dinner_status(guests, n);
    });
    status.join();

    for (int i = 0; i < n; i++) {
        t[i].join();
    }





}