BIN_FILES  = editor broker topic_server

CC = gcc


CFLAGS = -Wall -g -O2

LDFLAGS = -L$(INSTALL_PATH)/lib/
LDLIBS = -lpthread -lsqlite3


all: $(BIN_FILES)
.PHONY : all

editor: editor.o lines.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

broker: broker.o lines.o linked_list.o topic_server_clnt.o topic_server_xdr.o
	$(CC) $(LDFLAGS) $(CFLAGS) $^ $(LDLIBS) -o $@

topic_server: topic_server_server.o topic_server_svc.o topic_server_xdr.o
	$(CC) $(LDFLAGS) $(CFLAGS) $^ $(LDLIBS) -o $@

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

clean:
	rm -f $(BIN_FILES) *.o

.SUFFIXES:
.PHONY: clean
