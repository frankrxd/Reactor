#include "timer.h"

#include <unistd.h>
#include <errno.h>
#include <poll.h>
#include <sys/timerfd.h>
#include <iostream>
using std::cout;
using std::endl;

namespace project1
{
Timer::Timer(TimerCallBack && callback, int initialTime, int periodicTime)
: _fd(createTimerfd())
, _initialTime(initialTime)
, _periodicTime(periodicTime)
, _callback(std::move(callback))
, _isStarted(false){}

Timer::~Timer()
{
    if(_isStarted)
        stop();
}

void Timer::start()
{
    struct pollfd pfd;
    pfd.fd = _fd;
    pfd.events = POLLIN;

    _isStarted = true;
    setTimerfd(_initialTime,_periodicTime);

    while(_isStarted)
    {
        int ready = poll(&pfd,1,6000);
        if(ready == -1 && errno == EINTR)
            continue;
        else if(ready == -1)
        {
            perror("poll");
            exit(EXIT_FAILURE);
        }
        else if(ready == 0)
            cout<< ">> poll timeout! "<<endl;
        else {
            if(pfd.fd == _fd && (pfd.revents & POLLIN))
            {
                handleRead();
                if(_callback)
                    _callback();
            }
        }
    }
}

void Timer::stop()
{
    if(_isStarted)
    {
        _isStarted = false;
        setTimerfd(0,0);
    }
}

int Timer::createTimerfd()
{
    int fd = timerfd_create(CLOCK_REALTIME,0);
    if(fd == -1)
        perror("timerfd_create");
    return fd;
}

void Timer::setTimerfd(int initialTime,int periodicTime)
{
    struct itimerspec value;
    value.it_value.tv_sec = initialTime;
    value.it_value.tv_nsec = 0;
    value.it_interval.tv_sec = periodicTime;
    value.it_interval.tv_nsec = 0;

    int ret = timerfd_settime(_fd,0,&value,nullptr);
    if(ret == -1)
        perror("timerfd_settime");
}

void Timer::handleRead()
{
    uint64_t count = 0;
    int ret = read(_fd,&count,sizeof(uint64_t));
    if(ret != sizeof(count))
        perror("read");
}

}//end of namespace project1 
