#include "timer.hpp"
#include <chrono>

void timer_init(void)
{
}

void timer_final(void)
{
}

t_tick gettick(void)
{
    using namespace std::chrono;
    auto now = steady_clock::now();
    auto ms = duration_cast<milliseconds>(now.time_since_epoch());
    return static_cast<t_tick>(ms.count());
}