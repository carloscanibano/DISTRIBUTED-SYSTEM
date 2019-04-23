BIN_FILES  = editor broker

CC = gcc


CCGLAGS =	-Wall  -g

LDFLAGS = -L$(INSTALL_PATH)/lib/
LDLIBS = -lpthread


all: CFLAGS=$(CCGLAGS)
all: $(BIN_FILES)
.PHONY : all

editor: editor.o lines.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

broker: broker.o lines.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

clean:
	rm -f $(BIN_FILES) *.o

.SUFFIXES:
.PHONY : clean
