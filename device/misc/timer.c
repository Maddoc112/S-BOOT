/*
 * =====================================================================================
 *
 *       Filename:  timer.c
 *
 *    Description:  S3C2440时钟和定时器相关函数
 *
 *        Version:  1.0
 *        Created:  2008年11月23日 21时35分33秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Li Hongwang (mn), hoakee@gmail.com
 *        Company:  University of Science and Technology of China
 *
 * =====================================================================================
 */

#define TCFG0   *(volatile unsigned long *)(0x51000000)
#define TCON    *(volatile unsigned long *)(0x51000008)
#define TCNTB4  *(volatile unsigned long *)(0x5100003C)
#define TCNTO4  *(volatile unsigned long *)(0x51000040)  //Read Only

/* the PWM TImer 4 uses a counter of 15625 for 10 ms, so we need */
/* it to wrap 100 times (total 1562500) to get 1 sec. */
#define CONFIG_SYS_HZ  1562500

/*
 * PCLK = 50MHz, prescaler = 16 by TCFG0 = 0x0f00, default divider
 * value is 2. So counting 50/(2*16) = 1562500 costs 1 Sec, and 
 * 15625 costs 10 msec.
 */ 
int timer_load_val = 15625; 
static unsigned long timestamp;
static unsigned long lastdec;

int timer_init (void)
{
	/* use PWM Timer 4 because it has no output */
	/* prescaler for Timer 4 is 16 */
	TCFG0 = 0x0f00;
	if (timer_load_val == 0)
	{
	    timer_load_val = 15625;
	}
	/* load value for 10 ms timeout */
	TCNTB4 = timer_load_val;
	/* auto load, manual update of Timer 4 */
	TCON = (TCON & ~0x0700000) | 0x600000;
	/* auto load, start Timer 4 */
	TCON = (TCON & ~0x0700000) | 0x500000;
	
	lastdec = timer_load_val;
        timestamp = 0;

	return (0);
}

unsigned long get_timer_masked (void)
{
	unsigned long now = TCNTO4 & 0xffff;

	if (lastdec >= now) {
		/* normal mode */
		timestamp += lastdec - now;
	} else {
		/* we have an overflow ... */
		timestamp += lastdec + timer_load_val - now;
	}
	lastdec = now;

	return timestamp;
}

void msdelay(unsigned long msec)
{
    timer_init();

    unsigned long tmo, now;
    tmo = timer_load_val * msec;
    tmo = tmo / 10;

    unsigned long start = get_timer_masked();
    do
    {
        now = get_timer_masked();
    }
    while ( (now - start) < tmo );
}

/*
void udelay_masked (unsigned long usec)
{
	unsigned long tmo;
	unsigned long endtime;
	signed long diff;

	if (usec >= 1000) {
		tmo = usec / 1000;
		tmo *= (timer_load_val * 100);
		tmo /= 1000;
	} else {
		tmo = usec * (timer_load_val * 100);
		tmo /= (1000*1000);
	}

	endtime = get_timer_masked () + tmo;

	do {
		unsigned long now = get_timer_masked ();
		diff = endtime - now;
	} while (diff >= 0);
}


void udelay (unsigned long usec)
{
	unsigned long tmo;
	unsigned long start = get_timer(0);

	tmo = usec / 1000;
	tmo *= (timer_load_val * 100);
	tmo /= 1000;

	while ((unsigned long)(get_timer_masked () - start) < tmo)
	    //NO OP;
}
*/
void reset_timer_masked (void)
{
	/* reset time */
	lastdec = TCNTO4 & 0xffff;
	timestamp = 0;
}
