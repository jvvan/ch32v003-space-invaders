#include "time.h"
#include "ch32v003fun.h"

uint64_t get_current_time()
{
    return (int64_t)SysTick->CNT / DELAY_MS_TIME;
}