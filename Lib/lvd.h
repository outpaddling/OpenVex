
#ifndef __lvd_h__
#define __lvd_h__

#define LVD_464 0xe     /* 4.41V min/4.64V nom/4.87V max */
#define LVD_433 0xd     /* 4.11V min/4.33V nom/4.55V max */

/* lvd.c */
void lvd_set(unsigned char val);
unsigned char lvd_triggered(void);
unsigned char lvd_check(unsigned char val);

#endif

