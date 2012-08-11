CC = clang
LDFLAGS = -L/usr/local/lib -lcurl -larchive -lconfig -llua -lcrypto
CFLAGS = -c -I/usr/local/include
SOURCES = pkgr_download.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = pkgr

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $< -o $@
	chown root $@
	chmod ug+s $@
	chmod o-rx $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean

clean:
	rm -rf $(EXECUTABLE) $(OBJECTS)
