BINARY=kpsh
DIST=kp_shell
CODEDIRS=. lib
INCDIRS=. ./include/

CC=gcc
OPT=-O0

# Dependency flag to automatically build file having updated dependencies
DEPFLAGS=-MP -MD
# Common compiler flag
CFLAGS=-Wall -Wextra -g $(foreach D,$(INCDIRS),-I$(D)) $(OPT) $(DEPFLAGS)

CFILES=$(foreach D,$(CODEDIRS),$(wildcard $(D)/*.c))

OBJECTS=$(patsubst %.c,%.o,$(CFILES))
DEPFILES=$(patsubst %.c,%.d,$(CFILES))

all: $(BINARY)

$(BINARY): $(OBJECTS)
	$(CC) -o $@ $^

%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -rf $(BINARY) $(OBJECTS) $(DEPFILES) $(DIST).tgz

distribute: clean
	tar zcvf $(DIST).tgz *

diff:
	$(info The status of the repo, and the volume of per-file changes:)
	@git status
	@git diff --stat

-include $(DEPFILES)

.PHONY: all clean distribute diff
