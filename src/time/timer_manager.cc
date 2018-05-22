#include "timer_manager.h"

#include <sys/timerfd.h>
#include <unistd.h>
#include <memory.h>
#include "core/event_loop.h"

const int TimerManager::kInitTimeWheelSize = 60;
const int TimerManager::kInitMutexVecSize = 6;

TimerManager::TimerManager(EventLoop *loop, int size, int mutex_vec_size)
  : loop_(loop),
    timerfd_(CreateTimerfd()),
    timerfd_channel_(loop, timerfd_),
    circle_num_(0),
    tick_time_(1),
    spoke_index_(-1),
    timer_wheel_(size),
    mutex_vec_(mutex_vec_size),
    mutex_vec_size_(mutex_vec_size),
    size_(size)
{
    timerfd_channel_.set_readCallback(std::bind(&TimerManager::HandleRead, this));
    timerfd_channel_.EnableReading();
}

TimerManager::~TimerManager() {
    timerfd_channel_.DisableAll();
    timerfd_channel_.Remove();
    ::close(timerfd_);
    for (int i = 0;i < size_; i++) {
        for (auto iter = timer_wheel_[i].begin();iter != timer_wheel_[i].end();++iter)
        {
            delete (*iter); 
        }
    }
}

int TimerManager::CreateTimerfd() {
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC,
                                   TFD_NONBLOCK | TFD_CLOEXEC);
    if (timerfd < 0) {
        printf("fail in timerfd_create.\n");
    }
    SetTimerfd(timerfd);

    return timerfd;
}


void TimerManager::AddTimer(const TimerCallback &cb,
                             Timestamp when, double interval) {
    Timer *timer = new Timer(cb, when, interval);
    AddTimer(timer);
}

void TimerManager::AddTimer(Timer* timer)
{
    Timestamp when = timer->expiration();
    int time_sec = HowMuchTimeFromNow(when);
    //the right index to insert the time
    int time_wheel_time = size_ * tick_time_;
    int circle_add_num = (spoke_index_ + time_sec) / time_wheel_time;
    timer->set_circle_num(circle_num_ + circle_add_num);
    int index = (spoke_index_ + time_sec) % time_wheel_time;
    //add lock
    int mutex_index = index / (size_ / mutex_vec_size_ + 1);
    std::lock_guard<std::mutex> lock(mutex_vec_[mutex_index]);
    timer_wheel_[index].push_front(timer);
}

std::vector<Timer*> TimerManager::GetExpired(Timestamp now) {
    std::vector<Timer*> expired;
    //add lock
    int mutex_index = spoke_index_ / (size_ / mutex_vec_size_ + 1);
    std::lock_guard<std::mutex> lock(mutex_vec_[mutex_index]);

    if (timer_wheel_[spoke_index_].empty()) {
        return expired;
    }
    auto iter = timer_wheel_[spoke_index_].begin();
    while(iter != timer_wheel_[spoke_index_].end()) {
        if ((*iter)->circle_num() == circle_num_) {
            expired.push_back(*iter);
            timer_wheel_[spoke_index_].erase(iter++);
        } else {
            iter++;
        }
    }
    return expired;
}


void TimerManager::HandleRead()
{
    //move the spoke_index first
    spoke_index_++;
    if (spoke_index_ == size_) {
        spoke_index_ = spoke_index_ % size_;
        circle_num_++;
    }
    Timestamp now(Timestamp::Now());
    uint64_t timelong;
    ::read(timerfd_, &timelong, sizeof timelong); 
    std::vector<Timer*> expired = GetExpired(now);
    for (auto it = expired.begin(); it != expired.end(); ++it) {
        (*it)->Run();
    }
    printf("spoke_index:%d\n",spoke_index_);
    Reset(expired, now);
    SetTimerfd(timerfd_);
}


void TimerManager::Reset(const std::vector<Timer*> &expired, Timestamp now)
{
    for (auto it = expired.begin(); it != expired.end(); ++it) 
    {
        if ((*it)->repeat())
        {        
            (*it)->Restart(now);
            AddTimer(*it);
        } else {
            delete (*it);
        } 
    }
}

int TimerManager::HowMuchTimeFromNow(Timestamp when) {
    int microseconds = when.micro_seconds() -
                           Timestamp::Now().micro_seconds();
    if (microseconds < 100) {
        microseconds = 100;
    }
    int ts = microseconds / Timestamp::kMicroSecondsPerSecond + 1;
    return ts;
}

void TimerManager::SetTimerfd(int timerfd) { 
    struct itimerspec newValue;
    bzero(&newValue, sizeof newValue);
    newValue.it_value.tv_sec = tick_time_ ;

    int ret = ::timerfd_settime(timerfd, 0, &newValue, NULL);
    if (ret) {
        printf("error:resetTimerfd().\n");
    }
}
