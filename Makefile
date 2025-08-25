main:
	cc src/configure.c src/rawtui.c -o configure
	./configure

install:
	sudo mv minesweeper-c /usr/bin/

clean:
	sudo rm ~/.local/bin/minesweeper-c
