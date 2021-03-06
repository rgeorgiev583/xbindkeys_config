# makefile crée par Laurent VUIBERT

CC=	gcc -O3 -Wall
STD=	_GNU_SOURCE
GTK=	`pkg-config  --cflags --libs gtk+-2.0`
GTK2=	`pkg-config  --cflags gtk+-2.0`
OBJS=	xbindkeys_config.o menu.o middle.o speedc.o
NOM=    xbindkeys_config

prefix=	/usr

.c.o: 
	$(CC) $(GTK2) -D$(STD) -c $<

all: main

main: $(OBJS)
	$(CC) $(GTK) $(OBJS) -o $(NOM)

clean:
	rm -f *.o */*.o */*~ core $(NOM) *~
	rm -rf .v*

install:
	cp $(NOM) $(DESTDIR)$(prefix)/bin/

unistall: rm $(DESTDIR)$(prefix)/bin/$(NOM)

#End Makefile 
