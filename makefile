BINDIR=bin
OBJDIR=obj
CC=gcc
CFLAGS=-g -Wall -Wextra -Werror -Wpedantic -fsanitize=address
LDFLAGS=-static-libasan


DEPS = Bytes_t.h statistics.h
_OBJS = Bytes_t.o statistics.o
OBJS = $(patsubst %,$(OBJDIR)/%,$(_OBJS))


.PRECIOUS: $(OBJDIR)/%.o
$(OBJDIR)/%.o: %.c $(DEPS) | $(OBJDIR)
	$(CC) -c -o $@ $< $(CFLAGS)

MAINS = $(patsubst %.c,$(BINDIR)/%,$(wildcard main*.c))
.PHONY: all
all: $(MAINS)

$(BINDIR)/%: %.c $(OBJS) | $(BINDIR)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

$(OBJDIR):
	@mkdir $@
$(BINDIR):
	@mkdir $@

.PHONY: clean
clean:
	rm -rf $(BINDIR)
	rm -rf $(OBJDIR)
