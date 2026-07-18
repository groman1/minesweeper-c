CFLAGS := -Wall -Wextra
LDFLAGS :=
INSTALL := install
RM := rm -f
ARGSPATH := .colorargs

main: CFLAGS += -O2
main: LDFLAGS += -O2
main: configure minesweeper

debug: CFLAGS += -g
debug: LDFLAGS += -g
debug: configure minesweeper

configure: src/rawtui.o src/configure.o
	@$(info LD $@)
	@$(CC) $^ $(LDFLAGS) -o $@
	@$(info CONFIGURE)
	@./configure 2> $(ARGSPATH)

minesweeper: CFLAGS += $(file < $(ARGSPATH))
minesweeper: src/rawtui.o src/minesweeper.o
	@$(info LD $@)
	@$(CC) $^ $(LDFLAGS) -o $@

install:
	@$(info INSTALL minesweeper)
	@$(INSTALL) minesweeper /bin/

clean: OBJECTFILES := $(patsubst src/%.c, src/%.o, $(wildcard src/*.c))
clean:
	@$(info RM$(OBJECTFILES) .colorargs)
	@$(RM) $(OBJECTFILES) .colorargs
	@$(info RM configure minesweeper)
	@$(RM) configure minesweeper

src/%.o: src/%.c
	@$(info CC $<)
	@$(CC) -c $< $(CFLAGS) -o $@
