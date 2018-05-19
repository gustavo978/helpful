#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

/*
 *	This wrapper will ease the invocation of newly added system calls
 *	Think of it as a part of glibc
 */

int virtualtimeunshare(unsigned long, int);
int getvirtualtimeofday(struct timeval *, struct timezone *);
int gettimeofday(struct timeval *, struct timezone *);
int settimedilationfactor(int, int);

