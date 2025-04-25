main:
	cc src/configure.c -lncurses -o configure
	./configure
	rm configure

install:
	sudo mv minesweeper-c ~/.local/bin/minesweeper-c

clean:
	sudo rm ~/.local/bin/minesweeper-c
