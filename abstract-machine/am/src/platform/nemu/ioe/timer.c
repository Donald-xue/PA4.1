#include <am.h>
#include <stdio.h>
#include <nemu.h>
#include <sys/time.h>
#include <time.h>

static uint64_t boot_time;

void __am_timer_init() {
	uint32_t high = * (volatile uint32_t*) (RTC_ADDR + 4);
    uint32_t low = * (volatile uint32_t*) (RTC_ADDR);
    uint64_t now = (uint64_t) high;
    now = now << 32;
	now = now + (uint64_t)low;
    boot_time = now;
//	boot_time = * (volatile uint64_t*) RTC_ADDR;
//	printf("boot us = %d\n", boot_time);
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
	uint32_t high = * (volatile uint32_t*) (RTC_ADDR + 4);
	uint32_t low = * (volatile uint32_t*) (RTC_ADDR);
	uint64_t now = (uint64_t) high;
	now = now << 32;
	now = now + (uint64_t)low;
	uptime->us = now;
//	printf("us = %d\n", ticks);
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
