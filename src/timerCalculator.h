/*
This file is part of MailParser package.
Writen by
        - Cantarella Danilo (https://github.com/Flyer-90);
        - Randazzo Filippo (https://github.com/filirnd);
        - Siragusa Sebastiano (https://github.com/sebysira);

Full MailParser is released by GPL3 licence.
*/


#include "config.h"

void executionTime(struct timeval);

/*Function that return the time of function execution*/
void executionTime(struct timeval start_time) {
	struct timeval end_time;
	gettimeofday(&end_time,NULL);
	double start_dtime=(double)start_time.tv_sec+(double)start_time.tv_usec/1000000.0;
	double end_dtime=(double)end_time.tv_sec+(double)end_time.tv_usec/1000000.0;
	double diff=end_dtime-start_dtime;
	printf("Execution time: %lf seconds\n",diff);
}


/*
This file is part of MailParser package.
Writen by
        - Cantarella Danilo (https://github.com/Flyer-90);
        - Randazzo Filippo (https://github.com/filirnd);
        - Siragusa Sebastiano (https://github.com/sebysira);

Full MailParser is released by GPL3 licence.
*/
