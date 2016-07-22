#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>
#include "../engine/engine2d.h"
#include "map.h"

static _S_MAP_OBJECT MapObject;

void mapeditor_init() 
{
	MapObject.m_header.m_nSkima = 1;
	MapObject.m_pBuf = NULL;
	puts("Text TileMap Editor v1.1");
}

void mapeditor_close() 
{

	if ( MapObject.m_pBuf ) {
		free(MapObject.m_pBuf);
	}

}

void mapeditor_dump() 
{
	map_dump( &MapObject,Default_TilePalette);
}

void mapeditor_dump2() 
{
	//dump2 sx sy w h
	
	int sx = atoi(strtok(NULL," "));
	int sy = atoi(strtok(NULL," "));
	int w = atoi(strtok(NULL," "));
	int h = atoi(strtok(NULL," "));
	
	printf("cut %d,%d,%d,%d \r\n",sx,sy,w,h);

	int ix,iy;

	char *pTile_pal;
	_S_MAP_OBJECT *pObj;

	pTile_pal = Default_TilePalette;
	pObj = &MapObject;

	for(iy=sy;iy<sy+h;iy++) {
		for(ix=sx;ix<sx+w;ix++) {
			if(ix < pObj->m_header.m_nWidth && iy < pObj->m_header.m_nHeight) {
				putchar(pTile_pal[
						pObj->m_pBuf[iy*pObj->m_header.m_nWidth + ix]
				]
				);	
			}
		}
		printf("\r\n");
	}

}

void mapeditor_new() 
{
	// new 8 4
	int nWidth = 
		atoi(strtok(NULL," "));
	int nHeight = 
		atoi(strtok(NULL," "));

	map_new( &MapObject,nWidth,nHeight);

}

void mapeditor_put()
{
	//put 1 2 1 (x y tile_index)
	int x,y,tile_index;
	x = atoi(strtok(NULL," "));
	y = atoi(strtok(NULL," "));
	tile_index = atoi(strtok(NULL," "));
	map_PutTile(&MapObject,x,y,tile_index);

}

void mapeditor_hline()
{
	//hline 1 1 (x tile_index)
	int xpos, tile_index;
	xpos = atoi(strtok(NULL," "));
	tile_index = atoi(strtok(NULL," "));

	for(int iy=0;iy<MapObject.m_header.m_nHeight;iy++){
		MapObject.m_pBuf[ iy*MapObject.m_header.m_nWidth + xpos ] = tile_index;
	}
}

void mapeditor_vline()
{
	int ypos, tile_index;
	ypos = atoi(strtok(NULL," "));
	tile_index = atoi(strtok(NULL," "));

	for(int ix=0;ix<MapObject.m_header.m_nWidth;ix++) {
		MapObject.m_pBuf[ ix + ypos*MapObject.m_header.m_nWidth ] = tile_index;
	}
}

void mapeditor_save() {
	//save filename
	char *pTemp = strtok(NULL," ");
	map_save(&MapObject,pTemp);
	puts("Save OK");

}

void mapeditor_load() {
	//load filename
	char *pTemp = strtok(NULL," ");
	map_load(&MapObject,pTemp);
	puts("Load OK");	

}

/*
int main()
{
		}

		else if(!strcmp(pTemp,"dump")) {
		}
		else if(!strcmp(pTemp,"new")) {
		}
		else if(!strcmp(pTemp,"put")) {
		}
		else if(!strcmp(pTemp,"hline")) {
		}
		else if(!strcmp(pTemp,"vline")) {
		}
		else if(!strcmp(pTemp,"save")) {
		}
		else if(!strcmp(pTemp,"load")) {
		}
		else if(!strcmp(pTemp,"tridraw_1")) {
			// tridraw_1 1(tile index)
			
			int nTileIndex = atoi(strtok(NULL," "));
			int nHeight = MapObject.m_header.m_nHeight;
			int nWidth = MapObject.m_header.m_nWidth;
					
			for(int iy=0;iy<nHeight;iy++) {
				for(int ix=0;(ix<iy) && (ix<nWidth);ix++) {
					MapObject.m_pBuf[ iy*nWidth +ix ]=nTileIndex;
				}
			}

		}

		
		else if(!strcmp(pTemp,"draw_cross")) {
			//draw_cross 1 2 1
			int x,y,tile_index;
			x = atoi(strtok(NULL," "));
			y = atoi(strtok(NULL," "));
			tile_index = atoi(strtok(NULL," "));
			
			MapObject.m_pBuf[ y*MapObject.m_header.m_nWidth + x ] = tile_index;
			MapObject.m_pBuf[ y*MapObject.m_header.m_nWidth + (x+1) ] = tile_index;
			MapObject.m_pBuf[ y*MapObject.m_header.m_nWidth + (x-1) ] = tile_index;
			MapObject.m_pBuf[ (y-1)*MapObject.m_header.m_nWidth + x ] = tile_index;
			MapObject.m_pBuf[ (y+1)*MapObject.m_header.m_nWidth + x ] = tile_index;






		}



	}

	return 0;
}
*/

