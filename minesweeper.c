#include <curses.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

struct field
{
    int mines[20];
};

struct field field;

int genrandom();
int genfield();
int clrgetch();
int checkmines(int location);
int draw_frame(int maxx, int maxy);
int contains(int arr[], int chkval, int len);


int main()
{
    initscr();
    if(!has_colors()) {endwin();return 1;}
    start_color();
    curs_set(2);
    noecho();
    keypad(stdscr,1);

    int curslocation=0;
    int mines = 0;
    //WINDOW *window = newwin(10,10,1,1);


    genfield();
    //for(int i = 0; i<20;++i ,printf("%d\n", field.mines[i]));
    int end = 0;
    int maxx,maxy;
    getmaxyx(stdscr, maxy, maxx);
    draw_frame(maxx, maxy);

    while(!end)
    {
        while(!end){
            move(curslocation/10+maxy/2-5,curslocation%10+maxx/2-5);
            int ch = getch();
            if(curslocation/10>0&&ch==259){curslocation-=10;}
            else if(curslocation/10<9&&ch==258){curslocation+=10;}
            else if(curslocation%10>0&&ch==260){curslocation-=1;}
            else if(curslocation%10<9&&ch==261){curslocation+=1;}
            else if(ch==330){endwin();return 0;}
            else if(ch==32){break;}
            else if(ch==331){printw("M");}
        }
        if((mines = checkmines(curslocation))!=-1)
        {
            printw("%d", mines);
        }
        else
        {
            printw("+");
            mvprintw(maxy/4,maxx/2-4,"You lost");
            curs_set(0);
            getch();    
            end=1;        
        }
        
    }
    //printf("%d", curslocation);
    endwin();

}

int genfield()
{
    int buff;
    for (int i = 0; i<20; ++i)
    {
        buff = genrandom();
        if(!contains(field.mines, buff, i)){field.mines[i]=buff;}
        else(--i);
    }
    
    return 0;

}

int genrandom(){
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME,&ts);
    srand(ts.tv_nsec);
    return (rand()/(INT32_MAX/1000)-1)/10;
}

int contains(int arr[], int chkval, int len)
{
    for(int i = 0; i<len; ++i)
    {
        if(arr[i]==chkval){
            return 1;
        }
    }
    return 0;
}

int checkmines(int location)
{
    int returnmines = 0;

    if(contains(field.mines, location, 20)){return -1;}

    if(contains(field.mines, location-11, 20)){returnmines++;}
    if(contains(field.mines, location-10, 20)){returnmines++;}
    if(contains(field.mines, location-9, 20)){returnmines++;}
    if(contains(field.mines, location-1, 20)){returnmines++;}
    if(contains(field.mines, location+1, 20)){returnmines++;}
    if(contains(field.mines, location+9, 20)){returnmines++;}
    if(contains(field.mines, location+10, 20)){returnmines++;}
    if(contains(field.mines, location+11, 20)){returnmines++;}
    return returnmines;
}

int clrgetch()
{
    while(!getch());
}

int draw_frame(int maxx, int maxy)
{
    mvprintw(maxy/2-6,maxx/2-6, "============");
    for(int i = 0;i<10;++i){
        mvprintw(maxy/2-5+i,maxx/2-6, "|");
        mvprintw(maxy/2-5+i,maxx/2+5, "|");
    }
    mvprintw(maxy/2+6,maxx/2-6, "============");    
}