#include <stdio.h>

int main(int argc, char **argv)
{
	if (argc!=5) return 1;
	int h, w, p1, p2;
	sscanf(argv[1], "%d", &h);
	sscanf(argv[2], "%d", &w);
	sscanf(argv[3], "%d", &p1);
	sscanf(argv[4], "%d", &p2);

	for (int i = 0; i<h; ++i)
	{
		for (int t = 0; t<w; ++t)
		{
			if (p1 == w*i+t || p2 == w*i+t) putchar('*');
			else putchar(' ');
		}
		putchar('\n');
	}
}
