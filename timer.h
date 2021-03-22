#ifndef TIMER
#define TIMER
#include <string>
#include <chrono>
using namespace std;
class Timer{
    private:
        chrono::time_point<chrono::system_clock> s;
    public:
        string get_now();
        void start();
        string seconds();
};
#endif
