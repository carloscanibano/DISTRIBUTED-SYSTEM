BIN_FILES  = editor broker

CC = gcc


CFLAGS = -Wall -g -O2

LDFLAGS = -L$(INSTALL_PATH)/lib/
LDLIBS = -lpthread


all: $(BIN_FILES)
.PHONY : all

editor: editor.o lines.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

broker: broker.o lines.o linked_list.o
	$(CC) $(LDFLAGS) $(CFLAGS) $^ $(LDLIBS) -o $@

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

clean:
	rm -f $(BIN_FILES) *.o

.SUFFIXES:
.PHONY: clean
