#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>

#include "engine2d.h"



char buffer[] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

int main()
{
	struct timespec work_timer;
	set_conio_terminal_mode();
	double last_tick = 0;
	double acc_tick = 0;

	int nCount = 0;
	int bLoop = 1;

	system("clear");

	while(bLoop) {

		if(kbhit() != 0) {
			char ch = getch();
			if(ch == 'q') {
				bLoop = 0;
			}
			printf("%d \r\n",(int)ch);
		}

		clock_gettime(CLOCK_MONOTONIC,&work_timer);

		double cur_tick = work_timer.tv_sec + (double)(work_timer.tv_nsec * 1e-9);
		double delta_tick = cur_tick - last_tick;
		acc_tick += delta_tick;
		
		last_tick = cur_tick;
		
		if(acc_tick > 1.0) {
			//printf("tick tok \r\n");
			acc_tick = 0;

//render 
			gotoxy(1,1);
			int x,y;
			for(y=0;y<16;y++) {
				for(x=0;x<16;x++) {
					if(buffer[16 * y + x] == 0 ) {
						putchar('.');
					}
					else {

						putchar('A');
					}
				}
				printf("\r\n");
			}

		}

	}

}
