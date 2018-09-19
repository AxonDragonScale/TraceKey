#ifndef TIMER_H
#define TIMER_H

#include <thread>
#include <chrono>

class Timer {
    std::thread timerThread;
    bool alive = false;
    long repeatCount = -1L;      // how many times to call
    long remainingCount = -1L;     // how many times it has already been called

    std::chrono::milliseconds interval = std::chrono::milliseconds(0);  // interval between calls
    std::function<void(void)> func = nullptr;   // http://www.cplusplus.com/reference/functional/function

    void sleepAndRun() {
        std::this_thread::sleep_for(interval);
        if(alive) {
            (getFunc())();
        }
    }

    void threadFunc() {
        if(repeatCount == infinite) {
            while(alive) {
                sleepAndRun();
            }
        } else {
            while(remainingCount--) {
                sleepAndRun();
            }
        }
    }

public:
    static const long infinite = -1L;

    Timer() {
    }

    Timer(const std::function<void(void)> &f) : func(f) {
    }

    Timer(const std::function<void(void)> &f, const unsigned long &i, const long repeat = Timer::infinite) :
                                        func(f), interval(std::chrono::milliseconds(i)), repeatCount(repeat) {
    }

    void start(bool async = true) {
        if(isAlive()) { // if thread is already running just return
            return;
        }

        alive = true;
        remainingCount = repeatCount;

        if(async) {
            timerThread = std::thread(threadFunc, this);
        } else {
            this->threadFunc();
        }
    }

    void stop() {
        alive = false;
        timerThread.join();
    }

    void setFunc(const std::function<void(void)> &f) {
        // needs to be const coz, std::function doesnt have a non-const copy constructor
        func = f;
    }

    bool isAlive() {
        return alive;
    }

    void setRepeatCount(const long r) {
        if(alive) {     // if thread is running don't tamper with repeatCount
            return;
        }

        repeatCount = r;
    }

    long getRemainingCount() const {
        return remainingCount;
    }

    long getRepeatCount() const {
        return repeatCount;
    }

    void setInterval(const unsigned long &i) {
        if(alive) {
            return;
        }

        interval = std::chrono::milliseconds(i);
    }

    unsigned long getInterval() const {
        return interval.count();
    }

    const std::function<void(void)> &getFunc() const {
        return func;
    }

};

#endif // TIMER_H
