# makefile crée par Laurent VUIBERT

CC=	gcc -O3 -Wall
STD=	_GNU_SOURCE
GTK=	`gtk-config  --cflags --libs`
GTK2=	`gtk-config  --cflags`
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
