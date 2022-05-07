#include <am.h>
#include <stdio.h>
#include <nemu.h>
#include <sys/time.h>
#include <time.h>

static uint64_t boot_time;

void __am_timer_init() {
	boot_time = * (volatile uint64_t*) RTC_ADDR;
//	printf("boot us = %d\n", boot_time);
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
	uint64_t now = * (volatile uint64_t*) RTC_ADDR;
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
