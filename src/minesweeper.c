#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "rawtui.h"

#define MARKED 4
#define NEARBY 3
#define EMPTY 2
#define MINE 1
#define UNOPENED 0

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

#define onthesamerow(id, row) (int)(((int)id/(int)fieldwidth==(int)row)&&((id>>31)==0)&&((int)id<fieldwidth*fieldheight))
#define getLocation(location, counter) (location-1-fieldwidth+counter%3+fieldwidth*(counter/3))
#define moveToLocation(location) move(location/fieldwidth+maxy/2-fieldheight/2, location%fieldwidth+maxx/2-fieldwidth/2)

struct field
{
    uint32_t *mines;
    uint8_t *heatmap;
    uint32_t **freetiles;
	uint32_t *freetileslengths;
    uint8_t qtyfreetiles;
};

uint16_t fieldheight, fieldwidth, minescount, maxx, maxy;
int selectedsquares;
struct field field;
uint8_t *fieldbuffer;

int genrandom();
void genfield();
void revealmines();
void generateheatmap();
void generateemptygroups();
uint8_t checkmines(int location);
int showclosetiles(int arraynum);
void drawFrame(int maxx, int maxy);
int findpath(int pos1, int pos2);
int handlechecktiles(int location);
uint8_t contains(uint32_t *arr, uint32_t chkval, uint32_t len);
void fillFieldBuffer();
void drawMarkerCount(int used, int total);

int main()
{
    printf("Select from the available presets:\n\
    small: 8 by 8, 10 mines\n\
    medium: 16 by 16, 40 mines\n\
    large: 24 by 24, 90 mines\n\
    manual: or choose your own settings: ");
    char presets[8];
	fgets(presets, 8, stdin);

    if (!strcmp(presets, "small\n")){
        fieldwidth = 8;
        fieldheight = 8;
        minescount = 10;
    }
    else if (!strcmp(presets, "medium\n"))
    {
        fieldwidth = 16;
        fieldheight = 16;
        minescount = 40;
    }
    else if (!strcmp(presets, "large\n"))
    {
        fieldwidth = 24;
        fieldheight = 24;
        minescount = 90;
    }
    else
    {
        printf("Enter the width of the playfield: ");
        scanf("%hu", &fieldwidth);
        printf("Enter the height of the playfield: ");
        scanf("%hu", &fieldheight);
        printf("Enter the quantity of mines: ");
        scanf("%hu", &minescount);
    }

    getTermXY(&maxy, &maxx);
    if (maxx<fieldwidth+2||maxy<fieldheight+2)
	{
		free(field.mines);
		printf("The field is too big, exiting...\n");
		return 1;
	}

    if (minescount>fieldheight*fieldwidth)
	{
		printf("Too many mines, exiting...\n");
		return 1;
	}

    initinline();
	initcolorpair(MINE, MINE1, MINE2); // mine
	initcolorpair(EMPTY, EMPTY1, EMPTY2); // empty
	initcolorpair(NEARBY, NEARBY1, NEARBY2); // close
	initcolorpair(MARKED, MARKED1, MARKED2); // marked

    int stop = 0;
    while (!stop)
    {
        clear();
		setcursor(1);
        uint32_t curslocation = 0, usedMarkers = 0;
        selectedsquares = 0;

		fieldbuffer = malloc(fieldheight*fieldwidth);
		fillFieldBuffer();

        genfield();
        generateheatmap();
        generateemptygroups();

        int cont = 1;
        drawFrame(maxx, maxy);
		drawMarkerCount(usedMarkers, minescount);

        while (cont)
        {
            while (1)
			{
                move(curslocation/fieldwidth+maxy/2-fieldheight/2,curslocation%fieldwidth+maxx/2-fieldwidth/2);
                uint8_t ch = inesc();
                if (curslocation/fieldwidth>0&&ch==188)
					curslocation -= fieldwidth; // UP
                else if (curslocation/fieldwidth<fieldheight-1&&ch==189)
					curslocation += fieldwidth; // DOWN
                else if (curslocation%fieldwidth>0&&ch==191)
					--curslocation; // LEFT
                else if (curslocation%fieldwidth<fieldwidth-1&&ch==190)
					++curslocation; // RIGHT
                else if (ch==3||ch==113) // q
				{
					clear();
					move(0,0);
					deinit();
					return 0;
				}
                else if (ch==32) break; // space
                else if (ch==182) // insert
				{
                    if (fieldbuffer[curslocation]=='M')
                    {
						--usedMarkers;
						drawMarkerCount(usedMarkers, minescount);
                        wrcolorpair(UNOPENED);
                        print(" ");
						fieldbuffer[curslocation] = ' ';
                        wrcolorpair(0);
                    }
                    else if (fieldbuffer[curslocation]==' ' && usedMarkers<minescount)
                    {
						++usedMarkers;
						drawMarkerCount(usedMarkers, minescount);
                        wrcolorpair(MARKED);
                        print("M");
						fieldbuffer[curslocation] = 'M';
                        wrcolorpair(0);
                    }
                } 
                else if (ch==114) //r
				{
					cont = 0;
					break;
				}
            }
			if (!cont) break;
            if (fieldbuffer[curslocation]==' ')
            {
                if (field.heatmap[curslocation]!=255)
                {
                    selectedsquares += handlechecktiles(curslocation);
                    setcursor(1);
                }
                else
                {
                    revealmines();
                    setcursor(0);
					moveprint(1, maxx/2-4,"You lost");
                    cont = 0;
					stop = in()!=114;
                }

            }

            if (selectedsquares==fieldwidth*fieldheight-minescount)
            {
                revealmines();
                moveprint(1, maxx/2-8, "Congratulations!");
                setcursor(0);
                cont = 0;
				stop = in()!=114;
            }

        }
        free(field.mines);
        free(field.heatmap);
		free(field.freetileslengths);
        for (int i = 0; i<field.qtyfreetiles; ++i)
            free(field.freetiles[i]);

        free(field.freetiles);
		free(fieldbuffer);
        field.qtyfreetiles = 0;
    }
	clear();
	setcursor(1);
	deinit();
    return 0;
}

void fillFieldBuffer()
{
	for (int i = 0; i<fieldwidth*fieldheight; ++i)
		fieldbuffer[i] = 32;
}

void genfield()
{
    field.mines = malloc(sizeof(uint32_t)*minescount);
    uint32_t buff;
    for (int i = 0; i<minescount; ++i)
    {
        buff = genrandom();
        if (!contains(field.mines, buff, i)) field.mines[i] = buff;
        else --i;
    }
}

int genrandom()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME,&ts);
    srand(ts.tv_nsec);
    int ret = rand()%(fieldwidth*fieldheight);  // maybe will be better
    return ret;
}

uint8_t contains(uint32_t *arr, uint32_t chkval, uint32_t len)
{
    for (int i = 0; i<len; ++i)
        if (arr[i]==chkval)
            return 1;
    return 0;
}

uint8_t checkmines(int location)
{
    int returnmines = 0;
    if (contains(field.mines, location, minescount)) return -1;

    for (int i = 0; i<9; ++i)
	{
		if (i==4) continue;
		if (onthesamerow(getLocation(location, i), location/fieldwidth+(i/3-1)))
			if (contains(field.mines, location-1-fieldwidth+i%3+fieldwidth*(i/3), minescount))
				++returnmines;
	}

    return returnmines;
}

int revealtilesinarr(int arraynum)
{
	wrcolorpair(EMPTY);
	for (int i = 0; i<field.freetileslengths[arraynum]; ++i)
	{
		moveToLocation(field.freetiles[arraynum][i]);
		print(" ");
		fieldbuffer[field.freetiles[arraynum][i]] = 'O';
	}
	wrcolorpair(0);
	return showclosetiles(arraynum) + field.freetileslengths[arraynum];
}

int handlechecktiles(int location)
{
    int arraynum;

    if (field.heatmap[location])
	{
        wrcolorpair(NEARBY);
		dprintf(STDOUT_FILENO, "%d", field.heatmap[location]);
		fieldbuffer[location] = 'O';
        wrcolorpair(0);
        return 1;
    }
    else
    {
        for (int i = 0; i<field.qtyfreetiles; ++i)
        {
            if (contains(field.freetiles[i], location, field.freetileslengths[i]))
            {
                arraynum = i;
                break;
            }
        }
        setcursor(0);
		moveToLocation(location);
        return revealtilesinarr(arraynum);
    }

}

void drawFrame(int maxx, int maxy)
{
    for (int i = 0; i<fieldwidth; ++i, moveprint(maxy/2-fieldheight/2-1,maxx/2-fieldwidth/2-1+i, "="));

    for (int i = 0; i<fieldheight+2; ++i)
	{
        moveprint(maxy/2-fieldheight/2+i-1, maxx/2-fieldwidth/2-2, "||");
        moveprint(maxy/2-fieldheight/2+i-1, maxx/2+fieldwidth/2+fieldwidth%2, "||");
    }

    for (int i = 0; i<fieldwidth; ++i, moveprint(maxy/2+fieldheight/2+fieldheight%2,maxx/2-fieldwidth/2-1+i, "="));
}

void drawMarkerCount(int used, int total)
{
	saveCursorPos();
	move(0,0);
	dprintf(STDOUT_FILENO, "%d/%d", used, total);
	loadCursorPos();
}

void generateheatmap()
{
    field.heatmap = malloc(fieldheight*fieldwidth);
    for (int i = 0; i<fieldheight*fieldwidth; ++i)
        field.heatmap[i] = checkmines(i);
}

void generateemptygroups()
{
	uint8_t found;
	uint32_t lastFound = -1;
    field.qtyfreetiles = 0;
	field.freetiles = NULL;
	field.freetileslengths = NULL;

	for (int i = 0; i<fieldwidth*fieldheight; ++i)
	{
		if (field.heatmap[i]==0) // empty tile found
		{
			found = 0;
			for (int b = 0; b<4; ++b)
			{
				if (!onthesamerow(getLocation(i, b), i/fieldwidth+(b/3-1))) continue;
				if (field.heatmap[getLocation(i, b)]==0)
				{
					for (int t = 0; t<field.qtyfreetiles; ++t)
					{
						if (contains(field.freetiles[t], getLocation(i, b), field.freetileslengths[t]))
						{
							if (t==lastFound) continue;
							if (found) 
							{
								// merge if multiple arrays are connected
								field.freetileslengths[lastFound] += field.freetileslengths[t];
								field.freetiles[lastFound] = realloc(field.freetiles[lastFound], sizeof(uint32_t)*(field.freetileslengths[lastFound]));
								for (int l = field.freetileslengths[t]; l>0; --l)
									field.freetiles[lastFound][field.freetileslengths[lastFound]-l] = field.freetiles[t][field.freetileslengths[t]-l];

								free(field.freetiles[t]);

								for (int l = t; l<field.qtyfreetiles-1; ++l)
								{
									field.freetiles[l] = field.freetiles[l+1];
									field.freetileslengths[l] = field.freetileslengths[l+1];
								}

								lastFound = t;
								field.freetiles = realloc(field.freetiles, sizeof(uint32_t*)*field.qtyfreetiles);
								field.freetileslengths = realloc(field.freetileslengths, sizeof(uint32_t)*field.qtyfreetiles);
								--field.qtyfreetiles;
								continue;
							}
							field.freetiles[t] = realloc(field.freetiles[t], (field.freetileslengths[t]+1)*sizeof(uint32_t));
							field.freetiles[t][field.freetileslengths[t]] = i;
							++field.freetileslengths[t];
							found = 1;
							lastFound = t;
						}
					}
					
				}
			}
			if (!found) // no tiles around found
			{
				field.freetiles = realloc(field.freetiles, (field.qtyfreetiles+1)*sizeof(uint32_t*));
				field.freetileslengths = realloc(field.freetileslengths, sizeof(uint32_t)*(field.qtyfreetiles+1));
				field.freetiles[field.qtyfreetiles] = malloc(sizeof(uint32_t));

				field.freetileslengths[field.qtyfreetiles] = 1;
				field.freetiles[field.qtyfreetiles][0] = i;

				++field.qtyfreetiles;
			}
			lastFound = -1;
		}
	}
}

void revealmines()
{
    wrcolorpair(MINE);
    for (int i = 0; i<minescount; ++i)
	{
        moveprint(maxy/2-fieldheight/2+field.mines[i]/fieldwidth, maxx/2-fieldwidth/2+field.mines[i]%fieldwidth, "+");
    }
    wrcolorpair(0);
}

int showclosetiles(int arraynum)
{
    setcursor(0);
    int detectcnt = 0;
    for (int i = 0; i<field.freetileslengths[arraynum]; ++i)
	{
		for (int f = 0; f<9; ++f)
		{
			if (onthesamerow(getLocation(field.freetiles[arraynum][i], f), field.freetiles[arraynum][i]/fieldwidth+(f/3-1)))
			{
				if (fieldbuffer[getLocation(field.freetiles[arraynum][i], f)]==' '||fieldbuffer[getLocation(field.freetiles[arraynum][i], f)]=='M')
				{
					++detectcnt;
					wrcolorpair(NEARBY);
					moveToLocation(getLocation(field.freetiles[arraynum][i], f));
					fieldbuffer[field.freetiles[arraynum][i]-1-fieldwidth+f%3+fieldwidth*(f/3)] = 'O';
					dprintf(STDOUT_FILENO, "%d", field.heatmap[getLocation(field.freetiles[arraynum][i], f)]);
					wrcolorpair(0);
				}
			}
		}
	}
    setcursor(1);
    return detectcnt;
}
