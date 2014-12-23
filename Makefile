CC=gcc
CFLAGS=-O2 -W -DCONFIG_BPG_VERSION=\"$(CONFIG_BPG_VERSION)\"

all: ifBPG
ifBPG: spi00in.o spiBPG_ex.o
	$(CC) $(LDFLAGS) -o ifBPG.spi -shared -s $^ -lbpg -Wl,--kill-at -Wl,-s

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) *.o
	