BINDIR=bin
OBJDIR=obj
CC=gcc
CFlags=-Wall -Werror -Wpedantic -std=c2x


DEPS = Bytes_t.h
_OBJS = Bytes_t.o
OBJS = $(patsubst %,$(OBJDIR)/%,$(_OBJS))


.PRECIOUS: $(OBJDIR)/%.o
$(OBJDIR)/%.o: %.c $(DEPS) | $(OBJDIR)
	$(CC) -c -o $@ $< $(CFLAGS)

MAINS = $(patsubst %.c,$(BINDIR)/%,$(wildcard main*.c))
.PHONY: all
all: $(MAINS)

$(BINDIR)/%: %.c $(OBJS) | $(BINDIR)
	$(CC) -o $@ $^ $(CFLAGS)

$(OBJDIR):
	@mkdir $@
$(BINDIR):
	@mkdir $@

.PHONY: clean
clean:
	rm -rf $(BINDIR)
	rm -rf $(OBJDIR)
