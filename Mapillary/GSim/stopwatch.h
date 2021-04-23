#pragma once

#ifdef UNIX
#include <sys/time.h>
#include <stdlib.h>
#else	
#include <ctime>
#endif


class Stopwatch
{
public:
	float total_time;
#ifdef UNIX
	timeval t1, t2;
#else
	float t1, t2;
#endif

	Stopwatch(): total_time(0) {}
	float get_time() { return total_time; }	

	void start() {
#ifdef UNIX
		gettimeofday(&t1, NULL); 
#else
		t1 = (float)clock();
#endif
	}

	void resume() { 
#ifdef UNIX
		gettimeofday(&t1, NULL); 
#else
		t1 = (float)clock();
#endif
	}

	void pause() {
#ifdef UNIX
		gettimeofday(&t2, NULL);
		total_time += (float)(t2.tv_sec-t1.tv_sec) + (float)(t2.tv_usec-t1.tv_usec)/1e6;
#else
		t2 = (float)clock();
		total_time += (t2 - t1)/CLOCKS_PER_SEC;
#endif		
	}

	void stop() {
#ifdef UNIX
		gettimeofday(&t2, NULL);
		total_time += (float)(t2.tv_sec-t1.tv_sec) + (float)(t2.tv_usec-t1.tv_usec)/1e6;
#else
		t2 = (float)clock();
		total_time += (t2 - t1)/CLOCKS_PER_SEC;
#endif	
	}

	void clear() {
		total_time = 0;
	}
};