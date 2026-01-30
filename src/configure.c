#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "rawtui.h"

#define HIGHLIGHT 1
#define UNHIGHLIGHT 2
#ifndef BIN
#define BIN 0
#endif

uint16_t maxx, maxy;

void printColors()
{
    if (maxy>12)
    {
        moveprint(4,maxx/4-3, "Black");
        moveprint(5,maxx/4-2, "Red");
        moveprint(6,maxx/4-3, "Green");
        moveprint(7,maxx/4-3, "Yellow");
        moveprint(8,maxx/4-2, "Blue");
        moveprint(9,maxx/4-4, "Magenta");
        moveprint(10,maxx/4-2, "Cyan");
        moveprint(11,maxx/4-3, "White");

        moveprint(4,maxx/4*3-3, "Black");
        moveprint(5,maxx/4*3-2, "Red");
        moveprint(6,maxx/4*3-3, "Green");
        moveprint(7,maxx/4*3-3, "Yellow");
        moveprint(8,maxx/4*3-2, "Blue");
        moveprint(9,maxx/4*3-4, "Magenta");
        moveprint(10,maxx/4*3-2, "Cyan");
        moveprint(11,maxx/4*3-3, "White");
    }
}

void printInstructions()
{
    if(maxy>15&&maxx>24){
        moveprint(2, maxx/4-2 , "Text");
        moveprint(2, maxx/4*3-4 , "Background");
        moveprint(maxy-3, maxx/2-12, "Arr Up/Dn-move up/down");
        moveprint(maxy-2, maxx/2-15, "Arr R/L-move between colors");
        moveprint(maxy-1, maxx/2-8, "Space - continue");
    }
}
/*
        BLACK
        RED
        GREEN
        YELLOW
        BLUE
        MAGENTA
        CYAN
        WHITE
*/

void print_preview(int *locations, char text)
{
    initcolorpair(4,*(locations), *(locations+1));
    wrcolorpair(4);
	move(maxy/2, maxx/2);
	dprintf(STDOUT_FILENO, "%c", text);
	wrattr(NORMAL);
}

void highlight_choice(int column, int index, uint8_t on)
{
	wrattr(on*REVERSE);
	wrcolorpair(0);
    switch (index)
    {
        case 0: moveprint(4,maxx/4-3+(maxx/4*2)*column, "Black"); break;
		case 1: moveprint(5,maxx/4-2+(maxx/4*2)*column, "Red"); break;
		case 2: moveprint(6,maxx/4-3+(maxx/4*2)*column, "Green"); break;
		case 3: moveprint(7,maxx/4-3+(maxx/4*2)*column, "Yellow"); break;
		case 4: moveprint(8,maxx/4-2+(maxx/4*2)*column, "Blue"); break;
		case 5: moveprint(9,maxx/4-4+(maxx/4*2)*column, "Magenta"); break;
		case 6: moveprint(10,maxx/4-2+(maxx/4*2)*column, "Cyan"); break;
		case 7: moveprint(11,maxx/4-3+(maxx/4*2)*column, "White"); break;
	}
	wrattr(NORMAL);
}

int choose_color(int *colorpos, int *currcolor, char previewchar)
{
    print_preview(colorpos, previewchar);
    uint16_t currkey;
    while ((currkey=in())!=32)
    {
        getTermXY(&maxy, &maxx);
        if(currkey==65&&colorpos[*currcolor]>0) //up
        {
            highlight_choice(*currcolor, colorpos[*currcolor], 0);
            if (colorpos[*currcolor]-1==colorpos[(*currcolor)==1?0:1])
            {
                colorpos[*currcolor] = colorpos[*currcolor]>1?colorpos[*currcolor]-2:colorpos[*currcolor];
            }
            else
            {
                --colorpos[*currcolor];
            }
            highlight_choice(*currcolor, colorpos[*currcolor], 1);
        }
        else if(currkey==66&&colorpos[*currcolor]<7) //down
        {
            highlight_choice(*currcolor, colorpos[*currcolor], 0);
            
            if (colorpos[*currcolor]+1==colorpos[(*currcolor)==1?0:1])
            {
                colorpos[*currcolor] = colorpos[*currcolor]<5?colorpos[*currcolor]+2:colorpos[*currcolor];
            }
            else
            {
                ++colorpos[*currcolor];
            }
            highlight_choice(*currcolor, colorpos[*currcolor], 1);
        }
        else if(currkey==67&&*currcolor==0) // RIGHT
        {
            ++(*currcolor);
            highlight_choice(*currcolor, colorpos[*currcolor], 1);
        }
        else if(currkey==68&&*currcolor==1) // LEFT
        {
            --(*currcolor);
            highlight_choice(*currcolor, colorpos[*currcolor], 1);
        }
		else if(currkey==114) // r
		{
			return 1;
		}
		else if(currkey==113||currkey==3)
		{
			setcursor(1);
			move(0,0);
			clear();
			deinit();
			exit(1);
		}
        
        print_preview(colorpos, previewchar);
    }
	return 0;
}

int main()
{
    init();

    getTermXY(&maxy, &maxx);
	mineselect:
	clear();
	setcursor(0);
	moveprint(0, maxx/2-16, "Minesweeper Color Configuration");
	moveprint(2, maxx/2-10, "Choose mine colors:"); 
	int colorpos[2] = {1,0}, currcolor = 0;
	int minecolors[2], emptycolors[2], nearbycolors[2], markedcolors[2];
	
	printInstructions();
	printColors();
	int currkey;
	highlight_choice(0, colorpos[0], 1);
	highlight_choice(1, colorpos[1], 1);

	choose_color(colorpos, &currcolor, '+');

	minecolors[0] = colorpos[0];
	minecolors[1] = colorpos[1];

	clear();

	moveprint(0, maxx/2-16, "Minesweeper Color Configuration");
	moveprint(2, maxx/2-13, "Choose empty tile colors:");
	colorpos[0] = 7, colorpos[1] = 2, currcolor = 0;
	
	printInstructions();
	printColors();
	highlight_choice(0, colorpos[0], 1);
	highlight_choice(1, colorpos[1], 1);

	if (choose_color(colorpos, &currcolor, ' ')) goto mineselect;

	emptycolors[0] = colorpos[0];
	emptycolors[1] = colorpos[1];

	clear();

	moveprint(0, maxx/2-16, "Minesweeper Color Configuration");
	moveprint(2, maxx/2-13, "Choose nearby tile colors:"); 
	colorpos[0] = 7, colorpos[1] = 3, currcolor = 0;
	
	printInstructions();
	printColors();
	highlight_choice(0, colorpos[0], 1);
	highlight_choice(1, colorpos[1], 1);

	if (choose_color(colorpos, &currcolor, '1')) goto mineselect;

	nearbycolors[0] = colorpos[0];
	nearbycolors[1] = colorpos[1];

	clear();

	moveprint(0, maxx/2-16, "Minesweeper Color Configuration");
	moveprint(2, maxx/2-13, "Choose marked tile colors:"); 
	colorpos[0] = 7, colorpos[1] = 4, currcolor = 0;
	
	printInstructions();
	printColors();
	highlight_choice(0, colorpos[0], 1);
	highlight_choice(1, colorpos[1], 1);

	if (choose_color(colorpos, &currcolor, 'M')) goto mineselect;

	markedcolors[0] = colorpos[0];
	markedcolors[1] = colorpos[1];

	clear();
	move(0,0);
	setcursor(1);
	deinit();

	char command[160];
	sprintf(command, "gcc src/minesweeper.c src/rawtui.c -o minesweeper-c -D MARKED1=%d -D MARKED2=%d -D NEARBY1=%d -D NEARBY2=%d -D EMPTY1=%d -D EMPTY2=%d -D MINE1=%d -D MINE2=%d", markedcolors[0], markedcolors[1], nearbycolors[0], nearbycolors[1], emptycolors[0], emptycolors[1], minecolors[0], minecolors[1]);
	system(command);
    return 0;
}
