main:
	cc src/configure.c src/rawtui.c -o configure
	./configure

debug:
	cc src/configure.c src/rawtui.c -o configure
	./configure debug

install:
	mv minesweeper-c /usr/bin/

clean:
	rm -f configure minesweeper-c

findpath_testing:
	cc src/findpath_testing.c -fsanitize=address -o findpath-testing
