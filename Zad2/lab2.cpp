#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <cstdlib>
#include <vector>

using namespace std;

enum State {
    Waiting =0,
    Reading =1,
    Thinking=2,
    Writing=3,
    StuckInAnExistentialDread=4,
};

struct Book {
    Book()
    {
        pageCount = 0;
        readCount = 0;
        canBePickedUp = true;
    }
    int pageCount;
    atomic <bool> canBePickedUp;
   atomic <int> readCount;
   mutex isWritten;
   condition_variable canBeWritten;
   condition_variable canBeRead;
};

class Writer {
public:
    Writer() : state(StuckInAnExistentialDread), timeInState(-1), number(-777), lock(mutex) {
    }

    Writer(int number) : state(StuckInAnExistentialDread), number(number), timeInState(-1), lock(mutex) {
    }

    void setThread(Book& book) {
        thread = std::make_unique<std::thread>(&Writer::doAction, this, std::ref(book));
    }

    void joinThread()
    {
        thread->join();
    }

    void doAction(Book& book) {
        while(true)
        {
            int action = rand() % 2 + 2;
            int duration;
            int pageToWrite;
            switch (action) {
                case Thinking:
                    state = Thinking;
                    duration = rand() % 5000 + 1000;
                    this_thread::sleep_for(chrono::milliseconds(duration));
                    break;
                case Writing:
                    if(book.pageCount == 0 && book.canBePickedUp)
                    {
                        book.canBePickedUp = false;
                        state = Writing;
                        duration = rand()% 2000 + 500;
                        book.isWritten.lock();
                        pageToWrite = rand()% 250 + 50;
                        this_thread::sleep_for(chrono::milliseconds(duration));
                        book.pageCount += pageToWrite;
                        book.isWritten.unlock();
                        book.canBeRead.notify_all();
                        break;
                    }
                    if(book.readCount < 3)
                    {
                        state = Waiting;
                        book.canBeWritten.wait(lock,[&](){return book.readCount == 3;});
                    }
                    state = Writing;
                    duration = rand()% 2000 + 500;
                    book.isWritten.lock();
                    this_thread::sleep_for(chrono::milliseconds(duration));
                    book.canBeRead.notify_all();
                    break;
                default:
                    state = StuckInAnExistentialDread;
                    break;
            }
        }
    }

    string getStringState(){
        switch (state) {
            case Waiting:
                return "Waiting";
            case Reading:
                return "Reading";
            case Writing:
                return "Writing";
            case Thinking:
                return "Thinking";
            case StuckInAnExistentialDread:
                return "StuckInAnExistentialDread";
            default:
                return "Unknown";
        }

    }

private:
    std::unique_ptr<std::thread> thread;
    State state;
    int number;
    int timeInState;
    mutex mutex;
    unique_lock<std::mutex> lock{mutex, std::defer_lock};
};

class Reader {
public:
    Reader() : state(StuckInAnExistentialDread), timeInState(-1), lock(mutex) {
    }

    Reader(int number) : state(StuckInAnExistentialDread), timeInState(-1), lock(mutex) {
    }

    void setThread(Book& book) {
        thread = std::make_unique<std::thread>(&Reader::doAction, this, std::ref(book));
    }

    void joinThread()
    {
        thread->join();
    }

    void doAction(Book &book) {
        while(true)
        {
            int action = rand() % 2 + 1;
            int duration;
            switch (action) {
                case Thinking:
                    state = Thinking;
                    duration = rand() % 500 + 100;
                    this_thread::sleep_for(chrono::milliseconds(duration));
                    break;
                case Reading:
                    if(!book.canBePickedUp || book.pageCount == 0)
                    {
                        state = Waiting;
                        book.canBeRead.wait(lock);
                    }
                    state = Reading;
                    duration = rand()% 2000 + 500;
                    this_thread::sleep_for(chrono::milliseconds(duration));
                    book.readCount++;
                    if(book.readCount == 3)
                    {
                        book.canBeWritten.notify_all();
                    }
                    break;
                default:
                    state = StuckInAnExistentialDread;
                    break;
            }
        }
    }

    string getStringState(){
        switch (state) {
            case Waiting:
                return "Waiting";
            case Reading:
                return "Reading";
            case Thinking:
                return "Thinking";
            case StuckInAnExistentialDread:
                return "StuckInAnExistentialDread";
            default:
                return "Unknown";
        }
    }

private:
    mutex mutex;
    unique_lock<std::mutex> lock{mutex, std::defer_lock};
    std::unique_ptr<std::thread> thread;
    State state;
    int timeInState;
};

void bookClubSimulator(vector<Reader>& readers, vector <Writer>& writers, Book& book){
    while(true)
    {
        for(auto &reader : readers)
        {
            cout << "Reader is " << reader.getStringState() << "\n";
        }
        for(auto &writer : writers)
        {
            cout << "Writer is " << writer.getStringState() << "\n";
        }
        this_thread::sleep_for(chrono::milliseconds(100));
        cout <<"\n";
        system("cls");
    }
}

int main() {
    int numReaders=5;
    int numWriters=2;

    //cin >> numReaders >> numWriters;

    vector<Reader> readers(numReaders);
    vector<Writer> writers(numWriters);

    Book book;

    for(int i = 0;i<numReaders;i++)
    {
        //std::thread t(&Reader::doAction, &readers[i], std::ref(book));
        readers[i].setThread(book);
    }

    for(int i = 0;i<numWriters;i++)
    {
        //std::thread t(&Writer::doAction, &writers.back(), std::ref(book));
        writers[i].setThread(book);
    }

    unique_ptr<thread> t = make_unique<thread>(bookClubSimulator,ref(readers),ref(writers),ref(book));

    t->join();

    for(int i = 0;i<numReaders;i++)
    {
        readers[i].joinThread();
    }

    for(int i = 0;i<numWriters;i++)
    {
        writers[i].joinThread();
    }
}