#include "main.h"
#include  "ntp.h"

#include <esp_sntp.h>

volatile suseconds_t uSec = 0;
volatile time_t seconds = 0;


//__Secs_to_tm from https://git.musl-libc.org/cgit/musl/tree/src/time/__secs_to_tm.c?h=v0.9.15
/* 2000-03-01 (mod 400 year, immediately after feb29 */
#define LEAPOCH (946684800LL + 86400*(31+29))

#define DAYS_PER_400Y (365*400 + 97)
#define DAYS_PER_100Y (365*100 + 24)
#define DAYS_PER_4Y   (365*4   + 1)

int secs_to_tm(long long t, struct tm *tm)
{
	long long days, secs;
	int remdays, remsecs, remyears;
	int qc_cycles, c_cycles, q_cycles;
	int years, months;
	int wday, yday, leap;
	static const char days_in_month[] = {31,30,31,30,31,31,30,31,30,31,31,29};

	/* Reject time_t values whose year would overflow int */
	if (t < INT_MIN * 31622400LL || t > INT_MAX * 31622400LL)
		return -1;

	secs = t - LEAPOCH;
	days = secs / 86400;
	remsecs = secs % 86400;
	if (remsecs < 0) {
		remsecs += 86400;
		days--;
	}

	wday = (3+days)%7;
	if (wday < 0) wday += 7;

	qc_cycles = days / DAYS_PER_400Y;
	remdays = days % DAYS_PER_400Y;
	if (remdays < 0) {
		remdays += DAYS_PER_400Y;
		qc_cycles--;
	}

	c_cycles = remdays / DAYS_PER_100Y;
	if (c_cycles == 4) c_cycles--;
	remdays -= c_cycles * DAYS_PER_100Y;

	q_cycles = remdays / DAYS_PER_4Y;
	if (q_cycles == 25) q_cycles--;
	remdays -= q_cycles * DAYS_PER_4Y;

	remyears = remdays / 365;
	if (remyears == 4) remyears--;
	remdays -= remyears * 365;

	leap = !remyears && (q_cycles || !c_cycles);
	yday = remdays + 31 + 28 + leap;
	if (yday >= 365+leap) yday -= 365+leap;

	years = remyears + 4*q_cycles + 100*c_cycles + 400*qc_cycles;

	for (months=0; days_in_month[months] <= remdays; months++)
		remdays -= days_in_month[months];

	if (years+100 > INT_MAX || years+100 < INT_MIN)
		return -1;

	tm->tm_year = years + 100;
	tm->tm_mon = months + 2;
	if (tm->tm_mon >= 12) {
		tm->tm_mon -=12;
		tm->tm_year++;
	}
	tm->tm_mday = remdays + 1;
	tm->tm_wday = wday;
	tm->tm_yday = yday;

	tm->tm_hour = remsecs / 3600;
	tm->tm_min = remsecs / 60 % 60;
	tm->tm_sec = remsecs % 60;

	return 0;
}


void sntpTimeSyncCB(struct timeval *tv)
{
    uSec = tv->tv_usec;
    seconds = tv->tv_sec;
    struct tm time;
    secs_to_tm(seconds,&time);
    time.tm_year += 1900;
    time.tm_mon += 1;

    ESP32Time *pRtc = GPS.getRtc();
    
    pRtc->setTime(
        time.tm_sec,
        time.tm_min,
        time.tm_hour,
        time.tm_mday,
        time.tm_mon,
        time.tm_year,
        uSec);
     
    //pRtc->setTimeStruct(time); //Don't use. Something in the structure causes the timeStamp to be off vs GPS
    GPS.rtcTimeSetExternally();
    log_d("NTP time: seconds: %d  uSec: %d", seconds, uSec);
    log_d("NTP UTC Time: %4d-%02d-%02d  %02d:%02d:%02d",
        time.tm_year, time.tm_mon, time.tm_mday,
        time.tm_hour, time.tm_min, time.tm_sec
    );
    log_d("RTC TimeStamp (from NTP) %d",/* pRtc->getLocalEpoch() */ GPS.getTimeStamp());   
}


void InitNTP()
{
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0,"pool.ntp.org");
    sntp_set_time_sync_notification_cb(sntpTimeSyncCB);
    sntp_init();
}