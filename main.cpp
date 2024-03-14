#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <cstdlib>
using namespace std;

bool free_forks[2000];

bool start = false;

struct philo
{
    philo()
    {
        status ="ded";
    }
    thread t;
    char* status;
    int index;
    int time;
    int left_fork_index;
    int right_fork_index;

    void dine(pthread_mutex_t& left, pthread_mutex_t& right)
    {
        while(true)
        {
            int duration = rand() % 1000 + 500;
            time = duration;
            this->status = "is thinking";
            this_thread::sleep_for(chrono::milliseconds(duration));
            status = "is hungry";
            time = duration;
            duration = 5000;
            pthread_mutex_timedlock(&left,NULL);
            pthread_mutex_timedlock(&right,NULL);
            free_forks[left_fork_index] = false;
            free_forks[right_fork_index] = false;
            duration = rand() % 1000 + 500;
            status = "is eating";
            time = duration;
            this_thread::sleep_for(chrono::milliseconds(duration));
            pthread_mutex_unlock(&left);
            pthread_mutex_unlock(&right);
            free_forks[left_fork_index] = true;
            free_forks[right_fork_index] = true;
        }
    }
};

string number_to_string(int number)
{
    string parsed="";
    while(number>0)
    {
        parsed+='0'+number%10;
        number/=10;
    }
    string reversed="";
    for (int i = parsed.length() - 1; i >= 0;i--)
    {
        reversed+=parsed[i];
    }
    return reversed;
}

void dinner_status(philo *guests, int n)
{
    while(true)
    {
        this_thread::sleep_for(chrono::milliseconds(333));
        system("cls"); // Assuming Unix-like systems, you may need to adjust for Windows

        // Print philosopher status
        for (int i = 0; i < n; i++)
        {
            cout << "Philosopher " << guests[i].index << " is " << guests[i].status << " for " << guests[i].time << " milliseconds." << endl;
        }

        // Print fork availability
        cout << "Fork availability: ";
        for(int i = 0 ; i < n * 2; i+=2)
        {
            cout << (free_forks[i] ? "1" : "0") << " ";
        }
        cout << endl;

        // Clear console after each status update
        this_thread::sleep_for(chrono::milliseconds(1000)); // Wait for 1 second before clearing console
        system("cls");
    }
}

int main()
{
    int n = 5;

    philo *guests = new philo[n];
    pthread_mutex_t *forks = new pthread_mutex_t [n];

    for (int i = 0; i< n; i++)
    {
        pthread_mutex_init(&forks[i],NULL);
        int left_index = (i+1<n) ? i: 0;
        int right_index = (i+1<n)?i+1:i;
        guests[i].left_fork_index = left_index;
        guests[i].right_fork_index = right_index;
        guests[i].index = i + 1;
        guests[i].t = thread([&]
                             {
                                 guests[i].dine(forks[left_index],forks[right_index]);
                             });

    }

    thread status([&,n]{
        dinner_status(guests,n);
    });

    for( int i =0; i< n;i++)
    {
        guests[i].t.join();
    }

    start = true;

    status.join();

}