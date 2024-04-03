#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <ctime>
#include <cstdlib>
#include <vector>

using namespace std;

enum State {
    Reading,
    Thinking,
    Writing,
    StuckInAnExistentialDread,
};

struct Book {
    mutex *number;
    int pageCount;
};

class Writer {
public:
    Writer() : state(StuckInAnExistentialDread), timeInState(-1), number(-777) {
    }

    Writer(std::thread &thread, int number) : state(StuckInAnExistentialDread), number(number), timeInState(-1) {
        this->thread = std::move(thread);
    }

    void doAction(Book *mutex) {
        switch (state) {
            case Reading:
                break;
            case Thinking:
                break;
            case Writing:
                int randomNumber
                write(book[randomNumber]);
                break;
            case StuckInAnExistentialDread:
                break;
        }
    }

private:
    thread thread;
    State state;
    int number;
    int timeInState;

    void write() {

    }
};

class Reader {
public:
    Reader() : state(StuckInAnExistentialDread), timeInState(-1), number(-777) {
    }

    Reader(std::thread &thread, int number) : state(StuckInAnExistentialDread), timeInState(-1) {
        this->thread = std::move(thread);
    }

private:
    thread thread;
    State state;
    int timeInState;
    int number;
};

void bookCub() {

}

int main() {
    cout << "Enter: number of books / number of readers / numbers of writers\n";
    int numWriters, numBooks, numReaders;
    cin >> numBooks >> numReaders >> numWriters;

    vector<Book> books;
    vector<Reader> readers;
    vector<Writer> writers;

    for (int i = 0; i < numBooks; i++) {
        Book tempBook(new mutex(), 1000);
        books.push_back(tempBook);
    }

    for (int i = 0; i < numReaders; i++) {
        Reader tempReader(new thread(),i);
        readers.push_back(tempReader);
    }

    for (int i = 0; i < numWriters; i++) {

    }
}