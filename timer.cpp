#include "timer.h"
using namespace std;

string Timer::get_now(){
    auto tmp=chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
    auto sec=to_string(tmp/1000);
    auto milli=to_string(tmp%1000);
    sec=sec.substr(sec.size()-5, 5);
    milli=string(3-milli.size(), '0')+milli;
    return sec+"."+milli;
}
void Timer::start(){
    s=chrono::system_clock::now();
}
string Timer::seconds(){
    auto tmp=chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now()-s).count();
    auto sec=to_string(tmp/1000);
    auto milli=to_string(tmp%1000);
    milli=string(3-milli.size(), '0')+milli;
    return sec+"."+milli;
}
