#include <curses.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct field
{
    int *mines;
};

int fieldheight, fieldwidth, minescount;
struct field field;

int genrandom();
int genfield();
int revealmines();
int checkmines(int location);
int draw_frame(int maxx, int maxy);
int contains(int arr[], int chkval, int len);


int main()
{
    printf("Select from the available presets:\n\
    small: 8 by 8, 10 mines\n\
    medium: 16 by 16, 40 mines\n\
    large: 24 by 24, 90 mines\n\
    manual: or choose your own settings: ");
    char presets[8];
    scanf("%s", &presets);

    if(!strcmp(presets,"small")){
        fieldwidth = 8;
        fieldheight = 8;
        minescount = 10;
    }
    else if(!strcmp(presets,"medium"))
    {
        fieldwidth = 16;
        fieldheight = 16;
        minescount = 40;
    }
    else if(!strcmp(presets,"large"))
    {
        fieldwidth = 24;
        fieldheight = 24;
        minescount = 90;
    }
    else 
    {
        printf("Enter the width of the playfield: ");
        scanf("%d", &fieldwidth);
        printf("Enter the height of the playfield: ");
        scanf("%d", &fieldheight);
        printf("Enter the quantity of mines: ");
        scanf("%d", &minescount);
    }

    

    if(minescount>fieldheight*fieldwidth){printf("Too many mines, exiting..."); return 1;}

    initscr();
    noecho();
    keypad(stdscr,1);
    int stop = 0;
    while (!stop)
    {
        clear();
        curs_set(2);
        int curslocation = 0;
        int selectedsquares = 0;

        genfield();

        /*printw("\n");
        for(int i = 0; i<minescount;++i)
        {
            printw("%d\n", field.mines[i]);          //     For testing the mines locations
        }*/

        int cont = 1;
        int maxx,maxy;
        getmaxyx(stdscr, maxy, maxx);
        if(maxx<fieldwidth||maxy<fieldheight){printf("The field is too big, exiting...");free(field.mines);endwin();return 1;}
        draw_frame(maxx, maxy);

        while(cont)
        {
            while(cont){
                move(curslocation/fieldwidth+maxy/2-fieldheight/2,curslocation%fieldwidth+maxx/2-fieldwidth/2);
                int ch = getch();
                if(curslocation/fieldwidth>0&&ch==259){curslocation-=fieldwidth;} // UP
                else if(curslocation/fieldwidth<fieldheight-1&&ch==258){curslocation+=fieldwidth;} // DOWN
                else if(curslocation%fieldwidth>0&&ch==260){curslocation-=1;} // LEFT
                else if(curslocation%fieldwidth<fieldwidth-1&&ch==261){curslocation+=1;} // RIGHT
                else if(ch==330){endwin();free(field.mines); return 0;}
                else if(ch==32){break;}
                else if(ch==331){printw("M");}
                //mvprintw(0,0,"      ");
                //mvprintw(0,0,"%d",curslocation);  // DEBUG: for getting curret cursor location 
            }
            if(inch()==' ') {selectedsquares++;}
            if((checkmines(curslocation)!=-1))
            {
                printw("%d", checkmines(curslocation));
            }
            else
            {
                revealmines();
                mvprintw(1,maxx/2-4,"You lost");
                curs_set(0);
                cont = 0;
                if(getch()!=114) {stop = 1;}        
            }
            if(selectedsquares==fieldwidth*fieldheight-minescount)
            {
                revealmines();
                mvprintw(1,maxx/2-8,"Congratulations!");
                curs_set(0);
                cont = 0;
                if(getch()!=114) {stop = 1;}  
            }

        }
        free(field.mines);
    }
    endwin();
    return 0;
}

int genfield()
{
    field.mines = malloc(sizeof(int)*minescount);
    int buff;
    for (int i = 0; i<minescount; ++i)
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
    float ret = (float)rand()/INT32_MAX;  //should be something smaller than 1
    return (ret*fieldheight*fieldwidth);  
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

    if(contains(field.mines, location, minescount)){return -1;}

    

    // left border
    if((location+1)%fieldwidth==1){
        if(location==0){    //top left angle
            if(contains(field.mines, location+1, minescount)){returnmines++;}
            if(contains(field.mines, location+1+fieldwidth, minescount)){returnmines++;}
            if(contains(field.mines, location+fieldwidth, minescount)){returnmines++;}
        }
        else if(location==fieldheight*fieldwidth-fieldwidth){      //bottom left angle
            if(contains(field.mines, location+1, minescount)){returnmines++;}
            if(contains(field.mines, location+1-fieldwidth, minescount)){returnmines++;}
            if(contains(field.mines, location-fieldwidth, minescount)){returnmines++;}
        }
        else{   //just on left border
            if(contains(field.mines, location-fieldwidth, minescount)){returnmines++;}
            if(contains(field.mines, location-fieldwidth+1, minescount)){returnmines++;}
            if(contains(field.mines, location+1, minescount)){returnmines++;}
            if(contains(field.mines, location+1+fieldwidth, minescount)){returnmines++;}
            if(contains(field.mines, location+fieldwidth, minescount)){returnmines++;}
        }
    }
    else if((location+1)%fieldwidth==0)
    {
        if(location==fieldwidth-1) //top right angle
        {
            if(contains(field.mines, location-1, minescount)){returnmines++;}
            if(contains(field.mines, location+fieldwidth-1, minescount)){returnmines++;}
            if(contains(field.mines, location+fieldwidth, minescount)){returnmines++;}
        }
        else if (location+1==fieldwidth*fieldheight)    //bottom right angle
        {
            if(contains(field.mines, location-1, minescount)){returnmines++;}
            if(contains(field.mines, location-fieldwidth-1, minescount)){returnmines++;}
            if(contains(field.mines, location-fieldwidth, minescount)){returnmines++;}
        }
        else{   //just on the right border
            if(contains(field.mines, location-fieldwidth, minescount)){returnmines++;}
            if(contains(field.mines, location-fieldwidth-1, minescount)){returnmines++;}
            if(contains(field.mines, location-1, minescount)){returnmines++;}
            if(contains(field.mines, location-1+fieldwidth, minescount)){returnmines++;}
            if(contains(field.mines, location+fieldwidth, minescount)){returnmines++;}
        }
    }
    else{
        //nonborder
        if(contains(field.mines, location-1-fieldwidth, minescount)){returnmines++;}
        if(contains(field.mines, location-fieldwidth, minescount)){returnmines++;}
        if(contains(field.mines, location-fieldwidth+1, minescount)){returnmines++;}
        if(contains(field.mines, location-1, minescount)){returnmines++;}
        if(contains(field.mines, location+1, minescount)){returnmines++;}
        if(contains(field.mines, location+fieldwidth-1, minescount)){returnmines++;}
        if(contains(field.mines, location+fieldwidth, minescount)){returnmines++;}
        if(contains(field.mines, location+1+fieldwidth, minescount)){returnmines++;}
    }
    
    return returnmines;
}

int draw_frame(int maxx, int maxy)
{
    for(int i = 0; i<fieldwidth; ++i,mvprintw(maxy/2-fieldheight/2-1,maxx/2-fieldwidth/2-1+i, "="));

    for(int i = 0;i<fieldheight+2;++i){
        mvprintw(maxy/2-fieldheight/2+i-1,maxx/2-fieldwidth/2-2, "||");
        mvprintw(maxy/2-fieldheight/2+i-1,maxx/2+fieldwidth/2+fieldwidth%2, "||");
    }

    for(int i = 0; i<fieldwidth; ++i,mvprintw(maxy/2+fieldheight/2+fieldheight%2,maxx/2-fieldwidth/2-1+i, "="));
}

int revealmines()
{
    int maxx,maxy;
    getmaxyx(stdscr,maxy, maxx);

    for(int i = 0; i<minescount;++i){
        mvprintw(maxy/2-fieldheight/2+field.mines[i]/fieldwidth, maxx/2-fieldwidth/2+field.mines[i]%fieldwidth, "+");
    }
}