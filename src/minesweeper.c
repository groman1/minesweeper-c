#include <ncurses.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#define MARKED 4
#define NEARBY 3
#define EMPTY 2
#define MINE 1
#define NORMAL 5


#ifndef MARKED1

#define MARKED1 7
#define MARKED2 4
#define NEARBY1 7
#define NEARBY2 3 
#define EMPTY1 7
#define EMPTY2 2
#define MINE1 1
#define MINE2 0

#endif

struct field
{
    int *mines;
    unsigned char *heatmap;
    int **freetiles;
    int qtyfreetiles;
    int *shownfreetiles;
    int showncount;
};

int fieldheight, fieldwidth, minescount, maxx, maxy;
int selectedsquares;
struct field field;
int grouped = 0;
int coloravailable, counting;
int drawlocation[2];

int genrandom();
void genfield();
void revealmines();
void generateheatmap();
void generateemptygroups();
void activatecolorpair(int colorpair);
void deactivatecolorpair(int colorpair);
int checkmines(int location);
int showclosetiles(int arraynum);
void draw_frame(int maxx, int maxy);
int findpath(int pos1, int pos2);
int handlechecktiles(int location);
int contains(int *arr, int chkval, int len);


int main()
{
    printf("Select from the available presets:\n\
    small: 8 by 8, 10 mines\n\
    medium: 16 by 16, 40 mines\n\
    large: 24 by 24, 90 mines\n\
    manual: or choose your own settings: ");
    char presets[8];
    scanf("%s", presets);

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
    getmaxyx(stdscr, maxy, maxx);
    if(maxx<fieldwidth+2||maxy<fieldheight+2){printf("The field is too big, exiting...");free(field.mines);endwin();return 1;}
    if(has_colors()){
        coloravailable = 1;
        start_color();
        init_color(COLOR_RED, 700,0,0);
        init_color(COLOR_BLUE, 0,0,700);
        init_color(COLOR_YELLOW, 700,700,0);
        init_color(COLOR_WHITE, 800,800,800);

        init_pair(MINE,MINE1,MINE2);//mine
        init_pair(EMPTY, EMPTY1, EMPTY2);//empty
        init_pair(NEARBY, NEARBY1, NEARBY2);//close
        init_pair(MARKED, MARKED1, MARKED2);//marked
        init_pair(NORMAL, COLOR_WHITE, COLOR_BLACK);//normal
    }

    int stop = 0;
    while (!stop)
    {
        clear();
        curs_set(2);
        int curslocation = 0;
        selectedsquares = 0;

        field.showncount = 0;
        field.shownfreetiles = malloc(sizeof(int)*(field.showncount+1));

        genfield();
        generateheatmap();
        generateemptygroups();

        /*printw("\n");
        for(int i = 0; i<minescount;++i)
        {
            printw("%d\n", field.mines[i]);          //     For testing the mines locations
        }*/

        int cont = 1;
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
                else if(ch==113){endwin();free(field.mines); return 0;} //q
                else if(ch==32){break;} //space
                else if(ch==331){//insert
                    if(inch()==1101)
                    {
                        activatecolorpair(NORMAL);
                        printw(" ");
                        deactivatecolorpair(NORMAL);
                    }
                    else if (inch()==32||inch()==1312)
                    {
                        activatecolorpair(MARKED);
                        printw("M");
                        deactivatecolorpair(MARKED);
                    }
                } 
                else if(ch==114){cont=0;} //r
                //mvprintw(0,0,"   ");
                //mvprintw(0,0,"%d",curslocation);  // DEBUG: for getting curret cursor location
            }
            if(inch()==32||inch()==1312)
            {
                if(field.heatmap[curslocation]!=255)
                {
                    selectedsquares+=handlechecktiles(curslocation);
                    curs_set(2);
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
        free(field.shownfreetiles);
        for(int i = 0; i <= field.qtyfreetiles; ++i){
            free(field.freetiles[i]);
        }
        free(field.freetiles);
        field.qtyfreetiles = 0;
    }
    endwin();
    return 0;
}

void activatecolorpair(int colorpair)
{
    if (coloravailable)
    {
        attron(COLOR_PAIR(colorpair));
    }
}

void deactivatecolorpair(int colorpair)
{
    if (coloravailable)
    {
        attroff(COLOR_PAIR(colorpair));
    }
}

void genfield()
{
    field.mines = malloc(sizeof(int)*minescount);
    int buff;
    for (int i = 0; i<minescount; ++i)
    {
        buff = genrandom();
        if(!contains(field.mines, buff, i)){field.mines[i]=buff;}
        else(--i);
    }
}

int genrandom(){
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME,&ts);
    srand(ts.tv_nsec);
    int ret = rand()%(fieldwidth*fieldheight);  // maybe will be better
    return ret;
}

int checkifnull(int **ptr){
    if(ptr==NULL){return 1;}
    else{return 0;}
}

int checkifnulls(int *ptr)
{
    if(ptr==NULL){return 1;}
    else{return 0;}
}

int contains(int *arr, int chkval, int len)
{
    for(int i = 0; i<len; ++i)
    {
        if(arr[i]==chkval){
            return 1;
        }
    }
    return 0;
}

int onthesamerow(int id, int row)
{
	return (id/fieldwidth==row)&&(id>0)&&(id<fieldwidth*fieldheight);
}

int checkmines(int location)
{
    int returnmines = 0;
    if(contains(field.mines, location, minescount)){return -1;}

    for(int i = 0; i<9; ++i)
	{
		if (i==4) continue;
		if(onthesamerow(location-1-fieldwidth+i%3+fieldwidth*(i/3), location/fieldwidth+(i/3-1)))
		{
			if (contains(field.mines, location-1-fieldwidth+i%3+fieldwidth*(i/3), minescount)) ++returnmines;
		}
	}

    return returnmines;
}

void parseLocation(int location)
{
	drawlocation[0] = location/fieldwidth+maxy/2-fieldheight/2;
	drawlocation[1] = location%fieldwidth+maxx/2-fieldwidth/2;
}

int revealtilesinarr(int arraynum)
{
    if(field.showncount<=field.qtyfreetiles&&!contains(field.shownfreetiles, arraynum, field.showncount))
    {
        int* tempptr = NULL;
        while(checkifnulls(tempptr=realloc(field.shownfreetiles, (field.showncount+1)*sizeof(int))));
        field.shownfreetiles = tempptr;
        field.shownfreetiles[field.showncount] = arraynum;
        ++field.showncount;
        activatecolorpair(EMPTY);
        for (int i = 0;i<=field.freetiles[0][arraynum-1]; ++i){
			parseLocation(field.freetiles[arraynum][i]);
            mvprintw(drawlocation[0], drawlocation[1], " ");
        }
        deactivatecolorpair(EMPTY);
        return showclosetiles(arraynum)+field.freetiles[0][arraynum-1]+1;
    }
    else
    {
        return 0;
    }
}

int handlechecktiles(int location)
{
    int arraynum;

    if(field.heatmap[location]!=0){
        activatecolorpair(NEARBY);
        printw("%d", field.heatmap[location]);
        deactivatecolorpair(NEARBY);
        return 1;
    }
    else
    {
        for (int i = 1;i<=field.qtyfreetiles;++i)
        {
            if (contains(field.freetiles[i], location, field.freetiles[0][i-1]+1))
            {
                arraynum = i;
                break;
            }
        }
        curs_set(0);
		parseLocation(location);
        move(drawlocation[0], drawlocation[1]);
        return revealtilesinarr(arraynum);
    }

}

void draw_frame(int maxx, int maxy)
{
    for(int i = 0; i<fieldwidth; ++i,mvprintw(maxy/2-fieldheight/2-1,maxx/2-fieldwidth/2-1+i, "="));

    for(int i = 0;i<fieldheight+2;++i){
        mvprintw(maxy/2-fieldheight/2+i-1,maxx/2-fieldwidth/2-2, "||");
        mvprintw(maxy/2-fieldheight/2+i-1,maxx/2+fieldwidth/2+fieldwidth%2, "||");
    }

    for(int i = 0; i<fieldwidth; ++i,mvprintw(maxy/2+fieldheight/2+fieldheight%2,maxx/2-fieldwidth/2-1+i, "="));
}

void generateheatmap()
{
    field.heatmap = malloc(sizeof(unsigned char)*fieldheight*fieldwidth);
    //printw("   ");
    for (int i = 0;i < fieldheight*fieldwidth; ++i)
    {
        field.heatmap[i] = checkmines(i);
        //printw("%d ", field.heatmap[i]);     DEBUG: for getting the mines locations
    }
}

void generateemptygroups()
{
    int counter = 0;
    int **tempptr = NULL;
    int *tempptrs = NULL;

    field.qtyfreetiles = 0;
    field.freetiles = malloc(sizeof(int*)*(field.qtyfreetiles+1));
    field.freetiles[0] = malloc(sizeof(int)); // used arrays and their sizes
    field.freetiles[0][0] = -1;
    int found;

    while(counter<fieldheight*fieldwidth){
        if (field.heatmap[counter]==0){
            if(field.freetiles[0][0]==-1){   // just for the start
                while(checkifnull(tempptr = realloc(field.freetiles, (++field.qtyfreetiles+1)*sizeof(int*))));
                field.freetiles = tempptr;
                field.freetiles[1] = malloc(sizeof(int));
                field.freetiles[1][0] = counter;
                field.freetiles[0][0]=0;
            }
            else
            {
                found = 0;
                for(int i=1;(i<=field.qtyfreetiles)&&!found;++i)
                {
                    if(findpath(field.freetiles[i][0], counter)==1||findpath(counter, field.freetiles[i][0])==1)
                    {
                        tempptrs=NULL;
                        field.freetiles[0][i-1] = field.freetiles[0][i-1]+1;
                        while(checkifnulls(tempptrs = realloc(field.freetiles[i], (field.freetiles[0][i-1]+1)*sizeof(int))));
                        field.freetiles[i] = tempptrs;
                        field.freetiles[i][field.freetiles[0][i-1]] = counter;
                        ++found;
                        break;
                    }
                }
                if(found==0)
                {
                    tempptr = NULL;
                    tempptrs = NULL;
                    ++field.qtyfreetiles;
                    while(checkifnull(tempptr = realloc(field.freetiles, (field.qtyfreetiles+1)*sizeof(int*))));
                    field.freetiles = tempptr;
                    field.freetiles[field.qtyfreetiles] = malloc(sizeof(int));    //allocate a new array with one int(counter), increase qtyfreetiles (used arrays)
                    field.freetiles[field.qtyfreetiles][0]=counter;
                    while(checkifnulls(tempptrs = realloc(field.freetiles[0], field.qtyfreetiles*sizeof(int))));
                    field.freetiles[0] = tempptrs;
                    field.freetiles[0][field.qtyfreetiles-1]=0;
                }
            }
        }
        ++counter;
    }
}

int findpath(int pos1, int pos2)
{
    int curloc = pos1;
    int failedattempts = 0;
    int priorityxy = 0; // 0 - x, 1 - y
    int directionx = 0; // -1 - left, 1 - right
    int directiony = 0; // -1 - up, 1 - down
    int forceoverride = 0;
    int overridedirx = 0;
    int overridediry = 0;
    int flipped = 0;


    while(curloc!=pos2&&failedattempts<=fieldwidth)
    {

        directionx = forceoverride?overridedirx:(curloc%fieldwidth>pos2%fieldwidth?-1:1)*(curloc%fieldwidth!=pos2%fieldwidth);
        directiony = forceoverride?overridediry:(curloc/fieldwidth>pos2/fieldwidth?-1:1)*(curloc/fieldwidth!=pos2/fieldwidth);

        if (field.heatmap[curloc+directionx]!=0&&field.heatmap[curloc+directiony*fieldwidth]!=0)
        {
            forceoverride = 0;
            overridedirx = 0;
            overridediry = 0;
            failedattempts++;
            if (curloc%fieldwidth+1!=1-(-directionx>0?1:1-(-directionx==-1))&&-directiony>0?curloc%fieldwidth<=fieldheight-1:curloc>=fieldwidth) // if on left/right and bottom/top border
            {
                return 0;
            }
            else if (curloc-(fieldwidth+1)*(-directionx)>=fieldwidth-1&&field.heatmap[curloc-(fieldwidth+1)*(-directionx)]==0)
            {
                curloc = curloc - (fieldwidth+1)*(-directionx);
            }
            else if (curloc-(fieldwidth+1)*(directionx)>=fieldwidth-1&&field.heatmap[curloc-(fieldwidth+1)*(directionx)]==0)
            {
                curloc = curloc - (fieldwidth+1)*(directionx);
            }
            else if (field.heatmap[curloc-directionx]==0&&(directionx==1?curloc%fieldwidth<fieldwidth-2:directionx==-1?curloc%fieldwidth>=2:1))
            {
                if (directionx==0)
                {
                    if (curloc%fieldwidth+1==0&&field.freetiles[curloc+1]==0)
                    {
                        curloc++;
                        priorityxy=1;
                    }
                    else if (curloc%fieldwidth+1==1&&field.freetiles[curloc-1]==0)
                    {
                        curloc--;
                        priorityxy=1;
                    }
                }
                else
                {
                    curloc = curloc + directionx;
                    priorityxy = 1;
                }
            }
            else if (field.heatmap[curloc+directiony*fieldwidth]==0&&(directiony==1?curloc/fieldwidth<=fieldheight-2:directiony==-1?curloc/fieldwidth>=2:1))
            {

                if (directiony==0)
                {
                    if ((curloc/fieldwidth>=2||curloc/fieldwidth<=fieldwidth-2)&&field.freetiles[curloc+fieldwidth]==0)
                    {
                        curloc+=fieldwidth;
                        priorityxy=0;
                    }
                    else if ((curloc/fieldwidth>=2||curloc/fieldwidth<=fieldwidth-2)&&field.freetiles[curloc-fieldwidth]==0)
                    {
                        curloc-=fieldwidth;
                        priorityxy=0;
                    }
                }
                else
                {
                    curloc = curloc + directiony*fieldwidth;
                    priorityxy = 0;
                }
            }
            else{return 0;} // cant do anything
        }
        else if(field.heatmap[curloc+directionx]!=0&&directiony!=0&&(directiony>0?curloc%fieldwidth<fieldheight-1:curloc>fieldwidth)) // if not on top/bottom corner, prioritises going up/down
        {
            priorityxy = 1;
            overridedirx = 0;
            overridediry = 0;
            forceoverride = 0;
        }
        else if(field.heatmap[curloc+directiony*fieldwidth]!=0&&directionx!=0&&curloc%fieldwidth+1!=1-(directionx>0?1:0))  // if not on left/right border, prioritises going right/left
        {
            priorityxy = 0;
            overridedirx = 0;
            overridediry = 0;
            forceoverride = 0;
        }
        else if (field.heatmap[curloc+directionx]!=0&&directiony==0)
        {
            failedattempts++;
            forceoverride = 1;
            overridediry = overridediry!=0?overridediry:curloc<fieldwidth?1:curloc/fieldwidth<=fieldheight/2?1:-1;
            priorityxy = 1;
        }
        else if (field.heatmap[curloc+directiony*fieldwidth]!=0&&directionx==0)
        {
            failedattempts++;
            forceoverride = 1;
            overridedirx = overridedirx!=0?overridedirx:curloc%fieldwidth==0?1:curloc%fieldwidth<=fieldwidth/2?1:-1;
            priorityxy = 0;
        }
        else
        {
            forceoverride = 0;
        }

        //move

        if(priorityxy) // y
        {
            curloc += fieldwidth*directiony;
            if(!directiony&&!forceoverride){priorityxy = 0;}
            if(!directionx&&!forceoverride){priorityxy = 1;}
        }
        else if (!priorityxy) // x
        {
            curloc += directionx;
            if(!directiony&&!forceoverride){priorityxy = 0;}
            if(!directionx&&!forceoverride){priorityxy = 1;}
        }


        if((failedattempts==fieldwidth-1)&&!flipped&&(overridedirx!=0||overridediry!=0)){
            priorityxy=1;failedattempts=0;curloc=pos1;flipped=1;}
        else if (failedattempts==fieldwidth-1&&flipped){return 0;}

    }
    if(curloc==pos2) { return 1; }
    else { return 0; }
}

void revealmines()
{
    activatecolorpair(MINE);
    for(int i = 0; i<minescount;++i){
        mvprintw(maxy/2-fieldheight/2+field.mines[i]/fieldwidth, maxx/2-fieldwidth/2+field.mines[i]%fieldwidth, "+");
    }
    deactivatecolorpair(MINE);
}



int showclosetiles(int arraynum)
{
    curs_set(0);
    int detectcnt = 0;  
    for (int i = 0; i<=field.freetiles[0][arraynum-1];++i)
	{
		for (int f = 0; f<9;++f)
		{
			if(onthesamerow(field.freetiles[arraynum][i]-1-fieldwidth+f%3+fieldwidth*(f/3), field.freetiles[arraynum][i]/fieldwidth+(f/3-1)))
			{
				if(field.heatmap[field.freetiles[arraynum][i]-1-fieldwidth+f%3+fieldwidth*(f/3)]!=0)
				{
					parseLocation(field.freetiles[arraynum][i]-1-fieldwidth+f%3+fieldwidth*(f/3));
					if(mvinch(drawlocation[0], drawlocation[1])==' '||mvinch(drawlocation[0], drawlocation[1])=='M'||mvinch(drawlocation[0], drawlocation[1])==1312){detectcnt++;}
					activatecolorpair(NEARBY);
					mvprintw(drawlocation[0], drawlocation[1], "%d", field.heatmap[field.freetiles[arraynum][i]-1-fieldwidth+f%3+fieldwidth*(f/3)]);
					deactivatecolorpair(NEARBY);
				}
				else
				{
					if(!contains(field.freetiles[arraynum], field.freetiles[arraynum][i]-1-fieldwidth+f%3+fieldwidth*(f/3), field.freetiles[0][arraynum-1]+1))
					{
						for(int x = 1; x<=field.qtyfreetiles; ++x)
						{
							if (x==arraynum){continue;}
							if(contains(field.freetiles[x], field.freetiles[arraynum][i]-1-fieldwidth+f%3+fieldwidth*(f/3), field.freetiles[0][x-1]+1))
							{
								detectcnt+=revealtilesinarr(x);
								break;
							}
						}
					}
				}
			}
		}
	}
    curs_set(2);
    return detectcnt;
}
