#include <curses.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct field
{
    int *mines;
    unsigned char *heatmap;
    int **freetiles;
    int qtyfreetiles;
};

int fieldheight, fieldwidth, minescount, maxx, maxy;
struct field field;
int grouped = 0;

int genrandom();
int genfield();
int revealmines();
int generateheatmap();
int generateemptygroups();
int checkmines(int location);
int draw_frame(int maxx, int maxy);
int findpath(int pos1, int pos2);
int handlechecktiles(int location);
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
        generateheatmap();
        generateemptygroups();
        /*printw("\n");
        for(int i = 0; i<minescount;++i)
        {
            printw("%d\n", field.mines[i]);          //     For testing the mines locations
        }*/

        int cont = 1;
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
                //mvprintw(0,0,"   ");
                //mvprintw(0,0,"%d",curslocation);  // DEBUG: for getting curret cursor location 
            }
            if(inch()==' '||inch()=='M') 
            {
                if(field.heatmap[curslocation]!=255)
                {
                    selectedsquares+=handlechecktiles(curslocation);
                }
                else
                {
                    revealmines();
                    mvprintw(1,maxx/2-4,"You lost");
                    curs_set(0);
                    cont = 0;
                    if(getch()!=114) {stop = 1;}        
                }

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
        free(field.heatmap);
        for(int i = 0; i <= field.qtyfreetiles; ++i){
            free(field.freetiles[i]);
        }
        free(field.freetiles);
        field.qtyfreetiles = 0;
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

int parseLocation(int location, char type){
    if(type=='x'){
        return location%fieldwidth+maxx/2-fieldwidth/2;
    }
    else if (type=='y')
    {
        return location/fieldwidth+maxy/2-fieldheight/2;
    }
}

int handlechecktiles(int location)
{
    int arraynum;

    if(field.heatmap[location]!=0){
        printw("%d", field.heatmap[location]);
        return 1;
    }
    else
    {
        for (int i = 1;i<=field.qtyfreetiles;++i)
        {
            if (contains(field.freetiles[i], location, field.freetiles[0][i-1]))
            {
                arraynum = i;
                break;
            }
        }
        curs_set(0);
        for (int i = 0;i<field.freetiles[0][arraynum-1]; ++i){
            mvprintw(parseLocation(field.freetiles[arraynum][i], 'y'), parseLocation(field.freetiles[arraynum][i], 'x'), "0");
        }
        move(parseLocation(location, 'y'), parseLocation(location, 'x'));
        curs_set(2);
        return field.freetiles[0][arraynum-1];
    }
    
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

int generateheatmap()
{
    field.heatmap = malloc(sizeof(unsigned char)*fieldheight*fieldwidth);
    //printw("   ");
    for (int i = 0;i < fieldheight*fieldwidth; ++i)
    {
        field.heatmap[i] = checkmines(i);
        //printw("%d ", field.heatmap[i]);     DEBUG: for getting the mines locations
    }
}

int generateemptygroups()
{
    int counter = 0;
    size_t msize = 1;
    
    field.freetiles = malloc(sizeof(int)*msize);
    field.freetiles[0] = malloc(sizeof(int)); // used arrays and their sizes
    field.freetiles[0][0] = -1;
    field.qtyfreetiles = 0;
    while(counter<fieldheight*fieldwidth){
        if (field.heatmap[counter]==0){
            if(field.freetiles[0][0]==-1){   // just for the start
                field.freetiles = realloc(field.freetiles, (++msize)*sizeof(int));
                field.freetiles[1] = malloc(sizeof(int));
                field.freetiles[1][0] = counter;
                field.freetiles[0][0]=1;
                field.qtyfreetiles++;
            }
            else
            {
                int found = 0;
                for(int i=1;(i<=field.qtyfreetiles)&&!found;++i)
                {
                    for (int f=0; f<field.freetiles[0][i-1];++f)
                    {
                        if(findpath(counter, field.freetiles[i][f]))
                        {
                            field.freetiles = realloc(field.freetiles, (++msize)*sizeof(int));
                            field.freetiles[i] = realloc(field.freetiles[i], (field.freetiles[0][i-1])*sizeof(int)+sizeof(int));   //reallocate the array to increase size by +4 bytes for the new element, counter
                            field.freetiles[i][field.freetiles[0][i-1]] = counter;
                            field.freetiles[0][i-1]=field.freetiles[0][i-1]+1;
                            grouped++;
                            found++;    
                            break;
                        }
                    }
                }
                if(found==0)
                {
                    field.freetiles = realloc(field.freetiles, (++msize)*sizeof(int));
                    field.freetiles[++field.qtyfreetiles] = malloc(sizeof(int));    //allocate a new array with one int(counter), increase qtyfreetiles (used arrays)
                    field.freetiles[field.qtyfreetiles][0]=counter;
                    field.freetiles[0] = realloc(field.freetiles[0], (field.qtyfreetiles+1)*sizeof(int));
                    field.freetiles[0][field.qtyfreetiles-1]=field.freetiles[0][field.qtyfreetiles]+1;
                    grouped++;
                    //field.qtyfreetiles++;
                }
            }
            grouped--;
        }
        counter++;
    }
}

int findpath(int pos1, int pos2)
{
    int curloc = 0;
    int failedattempts = 0;
    curloc = pos1;
    int priorityxy = 0; // 0 - x, 1 - y
    int directionx = 0; // -1 - left, 1 - right
    int directiony = 0; // -1 - up, 1 - down
    while(curloc!=pos2&&failedattempts<3)
    {
        
        directionx = (curloc%fieldwidth>pos2%fieldwidth?-1:1)*(curloc%fieldwidth!=pos2%fieldwidth);
        directiony = (curloc/fieldwidth>pos2/fieldwidth?-1:1)*(curloc/fieldwidth!=pos2/fieldwidth);

        if (field.heatmap[curloc+directionx]!=0&&field.heatmap[curloc+directiony*fieldwidth]!=0)
        {
            failedattempts++;
            if(curloc-(fieldwidth+1)*(-directionx)>=fieldwidth-1&&field.heatmap[curloc-(fieldwidth+1)*(-directionx)]==0)
            {
                curloc = curloc - (fieldwidth+1)*(-directionx);
            }
            else if (curloc-(fieldwidth+1)*(directionx)>=fieldwidth-1&&field.heatmap[curloc-(fieldwidth+1)*(directionx)]==0)
            {
                curloc = curloc - (fieldwidth+1)*(directionx);
            }
            else if (field.heatmap[curloc-directionx]==0&&(directionx==1?curloc%fieldwidth<fieldwidth-2:curloc%fieldwidth>=2))
            {
                curloc = curloc - directionx*2;
                priorityxy = 1;
            }
            else if (field.heatmap[curloc+directiony*fieldwidth]==0&&(directiony==1?curloc/fieldwidth<=fieldheight-2:curloc/fieldwidth>=2))
            {
                curloc = curloc + directiony*fieldwidth*2;
                priorityxy = 0;
            }
            else{return 0;}
        }
        else if(field.heatmap[curloc+directionx]!=0&&directiony!=0)
        {
            priorityxy = 1;
        }
        else if(field.heatmap[curloc+directiony*fieldwidth]!=0&&directionx!=0)
        {
            priorityxy = 0;
        }

        //move 

        if(priorityxy)
        {
            curloc += fieldwidth*directiony;
            if(!directiony){priorityxy = 0;}
            if(!directionx){priorityxy = 1;}
        }
        else if (!priorityxy)
        {
            curloc += directionx;
            if(!directiony){priorityxy = 0;}
            if(!directionx){priorityxy = 1;}
        }
        
    }
    if(curloc==pos2) return 1;
    if(failedattempts==2) return 0;
}

int revealmines()
{
    for(int i = 0; i<minescount;++i){
        mvprintw(maxy/2-fieldheight/2+field.mines[i]/fieldwidth, maxx/2-fieldwidth/2+field.mines[i]%fieldwidth, "+");
    }
}