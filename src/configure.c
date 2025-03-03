#include <curses.h>
#include <stdlib.h>

#define HIGHLIGHT 1
#define UNHIGHLIGHT 2

int maxx, maxy;

void printColors()
{
    if (maxy>12)
    {
        mvprintw(4,maxx/4-3, "Black");
        mvprintw(5,maxx/4-2, "Red");
        mvprintw(6,maxx/4-3, "Green");
        mvprintw(7,maxx/4-3, "Yellow");
        mvprintw(8,maxx/4-2, "Blue");
        mvprintw(9,maxx/4-4, "Magenta");
        mvprintw(10,maxx/4-2, "Cyan");
        mvprintw(11,maxx/4-3, "White");

        mvprintw(4,maxx/4*3-3, "Black");
        mvprintw(5,maxx/4*3-2, "Red");
        mvprintw(6,maxx/4*3-3, "Green");
        mvprintw(7,maxx/4*3-3, "Yellow");
        mvprintw(8,maxx/4*3-2, "Blue");
        mvprintw(9,maxx/4*3-4, "Magenta");
        mvprintw(10,maxx/4*3-2, "Cyan");
        mvprintw(11,maxx/4*3-3, "White");
    }
}

void printInstructions()
{
    if(maxy>15&&maxx>24){
        mvprintw(2, maxx/4-2 , "Text");
        mvprintw(2, maxx/4*3-4 , "Background");
        mvprintw(maxy-3, maxx/2-12, "Arr Up/Dn-move up/down");
        mvprintw(maxy-2, maxx/2-15, "Arr R/L-move between colors");
        mvprintw(maxy-1, maxx/2-8, "Space - continue");
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
    init_pair(4,*(locations), *(locations+1));
    attron(COLOR_PAIR(4));
    mvprintw(maxy/2, maxx/2, "%c" , text);
    attroff(COLOR_PAIR(4));
}

void highlight_choice(int column, int index)
{
    if(index==0)
    {
        attron(COLOR_PAIR(HIGHLIGHT));
        mvprintw(4,maxx/4-3+(maxx/4*2)*column, "Black");
        attroff(COLOR_PAIR(HIGHLIGHT));
    }
    else if (index==1)
    {
        attron(COLOR_PAIR(HIGHLIGHT));
        mvprintw(5,maxx/4-2+(maxx/4*2)*column, "Red");
        attroff(COLOR_PAIR(HIGHLIGHT));

    }
    else if (index==2)
    {
        attron(COLOR_PAIR(HIGHLIGHT));
        mvprintw(6,maxx/4-3+(maxx/4*2)*column, "Green");
        attroff(COLOR_PAIR(HIGHLIGHT));

    }
    else if (index==3)
    {
        attron(COLOR_PAIR(HIGHLIGHT));
        mvprintw(7,maxx/4-3+(maxx/4*2)*column, "Yellow");
        attroff(COLOR_PAIR(HIGHLIGHT));
    }
    else if (index==4)
    {
        attron(COLOR_PAIR(HIGHLIGHT));
        mvprintw(8,maxx/4-2+(maxx/4*2)*column, "Blue");
        attroff(COLOR_PAIR(HIGHLIGHT));
    }
    else if (index==5)
    {
        attron(COLOR_PAIR(HIGHLIGHT));
        mvprintw(9,maxx/4-4+(maxx/4*2)*column, "Magenta");
        attroff(COLOR_PAIR(HIGHLIGHT));
    }
    else if (index==6)
    {       
        attron(COLOR_PAIR(HIGHLIGHT));
        mvprintw(10,maxx/4-2+(maxx/4*2)*column, "Cyan");
        attroff(COLOR_PAIR(HIGHLIGHT));
    }
    else if (index==7)
    {
        attron(COLOR_PAIR(HIGHLIGHT));
        mvprintw(11,maxx/4-3+(maxx/4*2)*column, "White");
        attroff(COLOR_PAIR(HIGHLIGHT));
    }
}

void unhighlight_choice(int column, int index)
{
    if(index==0)
    {
        attron(COLOR_PAIR(UNHIGHLIGHT));
        mvprintw(4,maxx/4-3+(maxx/4*2)*column, "Black");
        attroff(COLOR_PAIR(UNHIGHLIGHT));
    }
    else if (index==1)
    {
        attron(COLOR_PAIR(UNHIGHLIGHT));
        mvprintw(5,maxx/4-2+(maxx/4*2)*column, "Red");
        attroff(COLOR_PAIR(UNHIGHLIGHT));

    }
    else if (index==2)
    {
        attron(COLOR_PAIR(UNHIGHLIGHT));
        mvprintw(6,maxx/4-3+(maxx/4*2)*column, "Green");
        attroff(COLOR_PAIR(UNHIGHLIGHT));

    }
    else if (index==3)
    {
        attron(COLOR_PAIR(UNHIGHLIGHT));
        mvprintw(7,maxx/4-3+(maxx/4*2)*column, "Yellow");
        attroff(COLOR_PAIR(UNHIGHLIGHT));
    }
    else if (index==4)
    {
        attron(COLOR_PAIR(UNHIGHLIGHT));
        mvprintw(8,maxx/4-2+(maxx/4*2)*column, "Blue");
        attroff(COLOR_PAIR(UNHIGHLIGHT));
    }
    else if (index==5)
    {
        attron(COLOR_PAIR(UNHIGHLIGHT));
        mvprintw(9,maxx/4-4+(maxx/4*2)*column, "Magenta");
        attroff(COLOR_PAIR(UNHIGHLIGHT));
    }
    else if (index==6)
    {       
        attron(COLOR_PAIR(UNHIGHLIGHT));
        mvprintw(10,maxx/4-2+(maxx/4*2)*column, "Cyan");
        attroff(COLOR_PAIR(UNHIGHLIGHT));
    }
    else if (index==7)
    {
        attron(COLOR_PAIR(UNHIGHLIGHT));
        mvprintw(11,maxx/4-3+(maxx/4*2)*column, "White");
        attroff(COLOR_PAIR(UNHIGHLIGHT));
    }
}

void choose_color(int *colorpos, int *currcolor, char previewchar)
{
    print_preview(colorpos, previewchar);
    int currkey;
    while ((currkey=getch())!=32)
    {
        getmaxyx(stdscr, maxy, maxx);
        if(currkey==259&&colorpos[*currcolor]>0) //up
        {
            unhighlight_choice(*currcolor, colorpos[*currcolor]);
            if (colorpos[*currcolor]-1==colorpos[(*currcolor)==1?0:1])
            {
                colorpos[*currcolor] = colorpos[*currcolor]>1?colorpos[*currcolor]-2:colorpos[*currcolor];
            }
            else
            {
                --colorpos[*currcolor];
            }
            highlight_choice(*currcolor, colorpos[*currcolor]);
        }
        else if(currkey==258&&colorpos[*currcolor]<7) //down
        {
            unhighlight_choice(*currcolor, colorpos[*currcolor]);
            
            if (colorpos[*currcolor]+1==colorpos[(*currcolor)==1?0:1])
            {
                colorpos[*currcolor] = colorpos[*currcolor]<5?colorpos[*currcolor]+2:colorpos[*currcolor];
            }
            else
            {
                ++colorpos[*currcolor];
            }
            highlight_choice(*currcolor, colorpos[*currcolor]);
            //mvprintw(0,0,"%d %d", *colorpos, colorpos[1]);
        }
        else if(currkey==261&&*currcolor==0) // RIGHT
        {
            //unhighlight_choice(*currcolor, colorpos[*currcolor]);
            ++(*currcolor);
            highlight_choice(*currcolor, colorpos[*currcolor]);
        }
        else if(currkey==260&&*currcolor==1) // LEFT
        {
            //unhighlight_choice(*currcolor, colorpos[*currcolor]);
            --(*currcolor);
            highlight_choice(*currcolor, colorpos[*currcolor]);
        }
        
        print_preview(colorpos, previewchar);
        refresh();
        
    }
}

int main()
{
    initscr();
    noecho();
    keypad(stdscr,1);

    getmaxyx(stdscr, maxy, maxx);

    if(!has_colors())
    {
        curs_set(0);
        endwin();
        system("gcc src/minesweeper.c -lncurses -o minesweeper-c");
        return 0;
    }
    else
    {
        start_color();
        init_pair(1, COLOR_BLACK, COLOR_WHITE);
        init_pair(2, COLOR_WHITE, COLOR_BLACK);
        init_color(COLOR_YELLOW, 700,700,0);
        init_color(COLOR_WHITE, 800,800,800);

        curs_set(0);
        mvprintw(0, maxx/2-16, "Minesweeper Color Configuration");
        mvprintw(2, maxx/2-10, "Choose mine colors:"); 
        int colorpos[2] = {1,0}, currcolor = 0;
        int minecolors[2], emptycolors[2], nearbycolors[2], markedcolors[2];
        
        printInstructions();
        printColors();
        int currkey;
        highlight_choice(0, colorpos[0]);
        highlight_choice(1, colorpos[1]);

        choose_color(colorpos, &currcolor, '+');

        minecolors[0] = colorpos[0];
        minecolors[1] = colorpos[1];

        clear();
        refresh();

        mvprintw(0, maxx/2-16, "Minesweeper Color Configuration");
        mvprintw(2, maxx/2-13, "Choose empty tile colors:");
        colorpos[0] = 7, colorpos[1] = 2, currcolor = 0;
        
        printInstructions();
        printColors();
        highlight_choice(0, colorpos[0]);
        highlight_choice(1, colorpos[1]);

        choose_color(colorpos, &currcolor, ' ');

        emptycolors[0] = colorpos[0];
        emptycolors[1] = colorpos[1];

        clear();
        refresh();

        mvprintw(0, maxx/2-16, "Minesweeper Color Configuration");
        mvprintw(2, maxx/2-13, "Choose nearby tile colors:"); 
        colorpos[0] = 7, colorpos[1] = 3, currcolor = 0;
        
        printInstructions();
        printColors();
        highlight_choice(0, colorpos[0]);
        highlight_choice(1, colorpos[1]);

        choose_color(colorpos, &currcolor, '1');

        nearbycolors[0] = colorpos[0];
        nearbycolors[1] = colorpos[1];

        clear();
        refresh();

        mvprintw(0, maxx/2-16, "Minesweeper Color Configuration");
        mvprintw(2, maxx/2-13, "Choose marked tile colors:"); 
        colorpos[0] = 7, colorpos[1] = 4, currcolor = 0;
        
        printInstructions();
        printColors();
        highlight_choice(0, colorpos[0]);
        highlight_choice(1, colorpos[1]);

        choose_color(colorpos, &currcolor, 'M');

        markedcolors[0] = colorpos[0];
        markedcolors[1] = colorpos[1];

        clear();
        refresh();
        endwin();

        char *command;
        sprintf(command, "gcc src/minesweeper.c -lncurses -o minesweeper-c -D MARKED1=%d -D MARKED2=%d -D NEARBY1=%d -D NEARBY2=%d -D EMPTY1=%d -D EMPTY2=%d -D MINE1=%d -D MINE2=%d", markedcolors[0], markedcolors[1], nearbycolors[0], nearbycolors[1], emptycolors[0], emptycolors[1], minecolors[0], minecolors[1]);
        system(command);
        return 0;
    }
    endwin();
    return 1;
}