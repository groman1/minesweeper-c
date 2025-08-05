main:
	cc src/configure.c src/rawtui.c -o configure
	./configure

install:
	sudo mv minesweeper-c ~/.local/bin/minesweeper-c

clean:
	sudo rm ~/.local/bin/minesweeper-c
