#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "editor.h"

int main(int argc, char *argv[])
{

	mapeditor_init();

	int bLoop = 1;

	while(bLoop)
	{
		printf(">");
		char szCmd[32];
		gets(szCmd);
		char *pTemp = strtok(szCmd," ");
		if(!strcmp(pTemp,"exit")) {
			bLoop = 0;
			mapeditor_close();
		}
		else if(!strcmp(pTemp,"new")) {
			mapeditor_new();
		}
		else if(!strcmp(pTemp,"dump")) {
			mapeditor_dump();
		}
		else if(!strcmp(pTemp,"dump2")) {
			mapeditor_dump2();
		}
		else if(!strcmp(pTemp,"put")) {
			mapeditor_put();
		}
		else if(!strcmp(pTemp,"hline")) {
			mapeditor_hline();
		}
		else if(!strcmp(pTemp,"vline")) {
			mapeditor_vline();
		}
		else if(!strcmp(pTemp,"save")) {
			mapeditor_save();
		}
		else if(!strcmp(pTemp,"load")) {
			mapeditor_load();
		}
		else {
			puts("command not found");
		}

	}

	return 0;
}


