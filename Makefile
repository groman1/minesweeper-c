main:
	cc src/configure.c src/rawtui.c -o configure
	./configure

install:
	mv minesweeper-c /usr/bin/

clean:
	rm -f configure minesweeper-c
