#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>
#include <math.h>

#include "../engine/engine2d.h"
#include "../mapEditor/map.h"

#include "plane.h"
#include "bullet.h"
#include "alien.h"

struct timespec work_timer;
double acc_tick, last_tick;
int bLoop = 1;

static int nFSM = 0;
static int nStep = 0;

_S_MAP_OBJECT gScreenBuf[2];

_S_MAP_OBJECT gPlayerModel;
_S_MAP_OBJECT gBulletModel;
_S_MAP_OBJECT gBabyModel;
_S_MAP_OBJECT gPlayerBulletModel;
_S_MAP_OBJECT gTwoBulletModel;
_S_MAP_OBJECT gAlienModel;
_S_MAP_OBJECT gBossModel;
_S_MAP_OBJECT gClearModel;

_S_Plane gPlayerObject;
_S_BULLET_OBJECT gBulletObject[32];
_S_BULLET_OBJECT gBabyObject[32];
_S_BULLET_OBJECT gPlayerBulletObject[32];
_S_BULLET_OBJECT gTwoBulletObject[32];
_S_ALIEN_OBJECT gAlienObject[8];
_S_ALIEN_OBJECT gBossObject;
_S_Plane gClearObject;

double getDist(_S_BULLET_OBJECT *pBullet,_S_Plane *pPlane)
{
	double bullet_pos_x = pBullet->m_fXpos;
	double bullet_pos_y = pBullet->m_fYpos;

	double target_pos_x = pPlane->m_fXpos;
	double target_pos_y = pPlane->m_fYpos;

	double vx = target_pos_x - bullet_pos_x;
	double vy = target_pos_y - bullet_pos_y;
	double dist = sqrt(vx*vx+vy*vy);

	return dist;
}

double getDist2(_S_BULLET_OBJECT *pBullet,_S_ALIEN_OBJECT *pAlien)
{
	double bullet_pos_x = pBullet->m_fXpos;
	double bullet_pos_y = pBullet->m_fYpos;

	double target_pos_x = pAlien->m_fXpos;
	double target_pos_y = pAlien->m_fYpos;

	double vx = target_pos_x - bullet_pos_x;
	double vy = target_pos_y - bullet_pos_y;
	double dist = sqrt(vx*vx+vy*vy);

	return dist;
}

void mainTitle()
{
	switch(nStep) {
		case 0:
			setColor(36,40);
			printf("\r\n\r\n\r\n");
			printf("=============================\r\n\r\n");
			printf("   I N V A D E R   G A M E   \r\n\r\n");
			printf("=============================\r\n");
			printf("\r\n\r\n");
			puts(" - Press Any Key to Start - ");
			setColor(0,0);
			nStep = 1;
			break;
		case 1:
			if(kbhit() != 0) {
				char ch = getch();
				nFSM = 1;
				nStep = 0;
			}
			break;
	}
}

int main()
{

	for(int i=0;i<3;i++)
	{
		map_init(&gScreenBuf[i]);
		map_new(&gScreenBuf[i],50,50);
	}
	
	map_init(&gClearModel);
	map_load(&gClearModel,"clear.dat");

	map_init(&gPlayerModel);
	map_load(&gPlayerModel,"myplane.dat");

	map_init(&gPlayerBulletModel);
	map_load(&gPlayerBulletModel,"missile.dat");

	map_init(&gTwoBulletModel);
	map_load(&gTwoBulletModel,"two.dat");

	map_init(&gBulletModel);
	map_load(&gBulletModel,"plasma.dat");

	map_init(&gBabyModel);
	map_load(&gBabyModel,"baby.dat");

	map_init(&gAlienModel);
	map_load(&gAlienModel,"alien.dat");

	map_init(&gBossModel);
	map_load(&gBossModel,"boss.dat");

	//double TablePosition[] = {0,6.0,12};
	double TablePosition_x[] = {0,25,12};
	double TablePosition_y[] = {3,13,26};

	Plane_init(&gPlayerObject,&gPlayerModel,25,46);
	gPlayerObject.m_nFSM = 1;

	Plane_init(&gClearObject,&gClearModel,33,25);
	gClearObject.m_nFSM = 1;

	for (int i=0;i<3;i++)
	{
		_S_BULLET_OBJECT *pObj = &gBulletObject[i];
		bullet_init(pObj,0,0,0,&gBulletModel);
		pObj->m_nFSM = 1;
	}

	for (int i=0;i<3;i++)
	{
		_S_BULLET_OBJECT *pObj = &gBabyObject[i];
		bullet_init(pObj,0,0,0,&gBabyModel);
		pObj->m_nFSM = 1;
	}


	//for (int i=0;i<sizeof(gPlayerBulletObject)/sizeof(_S_BULLET_OBJECT);i++)
	for (int i=0;i<32;i++)
	{
		bullet_init(&gPlayerBulletObject[i],0,0,0,&gPlayerBulletModel);
	}

	for (int i=0;i<32;i++)
	{
		bullet_init(&gTwoBulletObject[i],0,0,0,&gTwoBulletModel);
	}

	for (int i=0;i<3;i++)
	{
		_S_ALIEN_OBJECT *pObj = &gAlienObject[i];
		alien_init(pObj,&gAlienModel);
		//pObj->m_fXpos = TablePosition[i];
		//pObj->m_fYpos = 2;
		pObj->m_fXpos = TablePosition_x[i];
		pObj->m_fYpos = TablePosition_y[i];
		pObj->m_nFSM = 1;

		gAlienObject[i].m_pBullet = &gBulletObject[i];
	}


	for (int i=0;i<3;i++)
	{
		alien_init(&gBossObject,&gBossModel);
		gBossObject.m_fXpos = 25;
		gBossObject.m_fYpos = 5;
		gBossObject.m_nFSM = 1;
	
		gBossObject.m_pBullet = &gBabyObject[i];
	}


	system("clear");

	set_conio_terminal_mode();
	acc_tick=last_tick=0;

	while(bLoop) {
		//타이밍처리
		clock_gettime(CLOCK_MONOTONIC,&work_timer);
		double cur_tick = work_timer.tv_sec +
			(double)(work_timer.tv_nsec * 1e-9);
		double delta_tick = cur_tick - last_tick;
		last_tick = cur_tick;


		if(nFSM == 0) {
			mainTitle(delta_tick);
		}

		else if(nFSM = 1) {


			//실시간 입력
			if(kbhit() != 0) {
				char ch = getch();
				if(ch == 'q') {
					bLoop = 0;
					puts("\r\n Bye~ \r");
				}
				else if(ch == 'j') {

					/*double bullet_pos_x = gPlayerObject.m_fXpos;
					  double bullet_pos_y = gPlayerObject.m_fYpos;

					  double target_pos_x = gAlienObject->m_fXpos;
					  double target_pos_y = gAlienObject->m_fYpos;

					  double vx = target_pos_x - bullet_pos_x;
					  double vy = target_pos_y - bullet_pos_y;
					  double dist = sqrt(vx*vx + vy*vy);
					  vx /=dist;
					  vy /=dist;

					  for (int i=0;i<32;i++) {
					  _S_BULLET_OBJECT *pObj = &gPlayerBulletObject[i];
					  if(pObj->m_nFSM == 0) {
					  pObj->pfFire(pObj,bullet_pos_x,bullet_pos_y,10,vx,vy,5.0);
					  break;
					  }
					  }*/

					for (int i=0;i<32;i++) {
						_S_BULLET_OBJECT *pObj = &gPlayerBulletObject[i];
						if(pObj->m_nFSM == 0) {
							pObj->pfFire(pObj,gPlayerObject.m_fXpos,gPlayerObject.m_fYpos,10,0,-1,3.0);
							break;
						}
					}


				}
				else if(ch== 'k') {
					double bullet_pos_x = gPlayerObject.m_fXpos;
					double bullet_pos_y = gPlayerObject.m_fYpos;

					double target_pos_x = gAlienObject->m_fXpos;
					double target_pos_y = gAlienObject->m_fYpos;

					double vx = target_pos_x - bullet_pos_x;
					double vy = target_pos_y - bullet_pos_y;
					double dist = sqrt(vx*vx + vy*vy);
					vx /=dist;
					vy /=dist;

					for (int i=0;i<32;i++) {
						_S_BULLET_OBJECT *pObj = &gTwoBulletObject[i];
						if(pObj->m_nFSM == 0) {
							pObj->pfFire(pObj,bullet_pos_x,bullet_pos_y,10,vx,vy,3.0);
							break;
						}
					}

				}


				gPlayerObject.pfApply(&gPlayerObject,delta_tick,ch);

			}
			
			gBossObject.pfApply(&gBossObject,delta_tick);


			for (int i=0;i<3;i++) 
			{
				_S_ALIEN_OBJECT *pObj = &gAlienObject[i];
				pObj->pfApply(pObj,delta_tick);
			}

			for (int i=0;i<3;i++)
			{
				_S_BULLET_OBJECT *pObj = &gBulletObject[i];
				pObj->pfApply(pObj,delta_tick);
			}


			for (int i=0;i<3;i++)
			{
				_S_BULLET_OBJECT *pObj = &gBabyObject[i];
				pObj->pfApply(pObj,delta_tick);
			}

			for (int i=0;i<32;i++) {
				_S_BULLET_OBJECT *pObj = &gPlayerBulletObject[i];
				pObj->pfApply(pObj,delta_tick);
			}		

			for (int i=0;i<32;i++) {
				_S_BULLET_OBJECT *pObj = &gTwoBulletObject[i];
				pObj->pfApply(pObj,delta_tick);
			}

			for(int i=0;i<3;i++)
			{
				_S_BULLET_OBJECT *pObj = &gBulletObject[i];
				if(pObj->m_nFSM != 0) {
					double dist = getDist(pObj,&gPlayerObject);

					if(dist < 1) {  //비행기랑 충돌하면 총알 sleep상태
						pObj->m_nFSM = 0;
						gPlayerObject.m_nFSM = 0;
						printf("\r\n - Gave Over - \r\n");
						bLoop = 0;
					}
				}
			}

			for(int i=0;i<3;i++)
			{
				_S_BULLET_OBJECT *pObj = &gBabyObject[i];
				if(pObj->m_nFSM != 0) {
					double dist = getDist(pObj,&gPlayerObject);

					if(dist < 1) {  //baby가 비행기랑 충돌하면 비행기 sleep상태
						pObj->m_nFSM = 0;
						gPlayerObject.m_nFSM = 0;
						printf("\r\n - Gave Over - \r\n");
						bLoop = 0;
					}
				}
			}




			for (int i=0;i<32;i++) {
				_S_BULLET_OBJECT *pObj = &gPlayerBulletObject[i];
				if(pObj->m_nFSM != 0) {
					double dist = getDist2(pObj,&gAlienObject[0]);

					if(dist < 0.7) {
						pObj->m_nFSM = 0;
						gAlienObject[0].m_nFSM = 0;
					}
				}
			}

			for (int i=0;i<32;i++) {
				_S_BULLET_OBJECT *pObj = &gPlayerBulletObject[i];
				if(pObj->m_nFSM != 0) {
					double dist = getDist2(pObj,&gAlienObject[1]);

					if(dist < 0.7) {
						pObj->m_nFSM = 0;
						gAlienObject[1].m_nFSM = 0;
					}
				}
			}

			for (int i=0;i<32;i++) {
				_S_BULLET_OBJECT *pObj = &gPlayerBulletObject[i];
				if(pObj->m_nFSM != 0) {
					double dist = getDist2(pObj,&gAlienObject[2]);

					if(dist < 0.7) {
						pObj->m_nFSM = 0;
						gAlienObject[2].m_nFSM = 0;
					}
				}
			}

			for (int i=0;i<32;i++) {
				_S_BULLET_OBJECT *pObj = &gTwoBulletObject[i];
				if(pObj->m_nFSM != 0) {
					double dist = getDist2(pObj,&gAlienObject[0]);

					if(dist < 0.7) {
						pObj->m_nFSM = 0;
						gAlienObject[0].m_nFSM = 0;
					}
				}
			}

			for (int i=0;i<32;i++) {
				_S_BULLET_OBJECT *pObj = &gTwoBulletObject[i];
				if(pObj->m_nFSM != 0) {
					double dist = getDist2(pObj,&gAlienObject[1]);

					if(dist < 0.7) {
						pObj->m_nFSM = 0;
						gAlienObject[1].m_nFSM = 0;
					}
				}
			}

			for (int i=0;i<32;i++) {
				_S_BULLET_OBJECT *pObj = &gTwoBulletObject[i];
				if(pObj->m_nFSM != 0) {
					double dist = getDist2(pObj,&gAlienObject[2]);

					if(dist < 0.7) {
						pObj->m_nFSM = 0;
						gAlienObject[2].m_nFSM = 0;
					}
				}
			}

			//보스 죽이기

			for (int i=0;i<32;i++) {
				_S_BULLET_OBJECT *pObj = &gPlayerBulletObject[i];
				if(pObj->m_nFSM != 0) {
					double dist = getDist2(pObj,&gBossObject);

					if(dist < 0.2) {
						pObj->m_nFSM = 0;
						gBossObject.m_nFSM = 0;
					}
				}
			}

			for (int i=0;i<32;i++) {
				_S_BULLET_OBJECT *pObj = &gTwoBulletObject[i];
				if(pObj->m_nFSM != 0) {
					double dist = getDist2(pObj,&gBossObject);

					if(dist < 0.2) {
						pObj->m_nFSM = 0;
						gBossObject.m_nFSM = 0;
					}
				}
			}

			//타이밍 계산
			acc_tick += delta_tick;


			if(acc_tick > 0.1) {
				gotoxy(0,0);
				map_drawTile(&gScreenBuf[0],0,0,&gScreenBuf[1]);
				gPlayerObject.pfDraw(&gPlayerObject,&gScreenBuf[1]);

				for(int i=0;i<3;i++) 

				{
					_S_ALIEN_OBJECT *pObj = &gAlienObject[i];
					pObj->pfDraw(pObj,&gScreenBuf[1]);
				}

				for(int i=0;i<3;i++) 
				{
					_S_BULLET_OBJECT *pObj = &gBulletObject[i];
					pObj->pfDraw(pObj,&gScreenBuf[1]);
				}


				for (int i=0;i<32;i++) {
					_S_BULLET_OBJECT *pObj = &gPlayerBulletObject[i];
					pObj->pfDraw(pObj,&gScreenBuf[1]);
				}

				for (int i=0;i<32;i++) {
					_S_BULLET_OBJECT *pObj = &gTwoBulletObject[i];
					pObj->pfDraw(pObj,&gScreenBuf[1]);
				}

				if(gAlienObject[0].m_nFSM==0 && gAlienObject[1].m_nFSM==0 && gAlienObject[2].m_nFSM==0)
				{
					gBossObject.pfDraw(&gBossObject,&gScreenBuf[1]);

					for(int i=0;i<3;i++) 
					{
						_S_BULLET_OBJECT *pObj = &gBabyObject[i];
						pObj->pfDraw(pObj,&gScreenBuf[1]);
					}
				}

				if(gBossObject.m_nFSM == 0)
				{
					gClearObject.pfDraw(&gClearObject,&gScreenBuf[1]);
					gPlayerObject.m_nFSM = 0;
				}

				map_dump(&gScreenBuf[1],Default_TilePalette);
				acc_tick = 0;
			}

		}
	}

	return 0;

}
