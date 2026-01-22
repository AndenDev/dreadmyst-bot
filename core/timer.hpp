#ifndef TIMER_HPP
#define TIMER_HPP

#include "cbasetypes.hpp"

typedef int64 t_tick;

void timer_init(void);
void timer_final(void);
t_tick gettick(void);

#endif