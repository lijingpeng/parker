PARKER_HOME= /var/parker
CGI_HOME= /var/www/cgi-bin/parker 
VALIDDAYS=30
#Define the user Language: english or chinese
CC= gcc

CFLAGS= -O2 -DNOSYSLOG -Wall

#LDFLAGS= -ldl
LDFLAGS=

EXES= config.h search statist siteinfo submit listfilter convertmask getdnsname collect lzo_comp flashdata

.c.o: $<
	$(CC) -c $(CFLAGS) $<

default: all

all: $(EXES)

install: default
	-cp search $(CGI_HOME)
	-cp statist $(CGI_HOME)
	-cp submit $(CGI_HOME)
	-cp siteinfo $(CGI_HOME)
	@if [ ! -d ../bin ]; then
		mkdir -p ../bin; \
	fi
	-cp getdnsname ../bin/.
	-cp listfilter ../bin/.
	-cp convertmask ../bin/.
	-cp lzo_comp ../bin/.
	-cp collect ../bin/.
	-cp flashdata ../bin/.
	
clean:
	-rm -f  $(EXES)

realclean: clean
	-(cd ../bin; rm -f $(EXES) )

siteinfo: siteinfo.c common.c parker.h language.h
	$(CC) $(CFLAGS) -o siteinfo siteinfo.c common.c $(LDFLAGS)

statist: statist.c common.c parker.h language.h
	$(CC) $(CFLAGS) -o statist statist.c common.c $(LDFLAGS)

search: search.c common.c log.c minilzo.c minilzo.h lzoconf.h lzo_fun.c parker.h config.h language.h
	$(CC) $(CFLAGS) -o search  search.c common.c log.c minilzo.c lzo_fun.c $(LDFLAGS)

collect: collect.c collect.h config.h
	$(CC) $(CFLAGS) -o collect collect.c
	
listfilter: listfilter.c common.c parker.h
	$(CC) $(CFLAGS) -o listfilter listfilter.c common.c $(LDFLAGS)

convertmask: convertmask.c parker.h
	$(CC) $(CFLAGS) -o convertmask convertmask.c $(LDFLAGS)

getdnsname: getdnsname.c common.c parker.h
	$(CC) $(CFLAGS) -o getdnsname getdnsname.c common.c $(LDFLAGS)

submit: submit.c parker.h common.c
	$(CC) $(CFLAGS) -o submit submit.c common.c log.c $(LDFLAGS)
	
lzo_comp: lzo_comp.c minilzo.c minilzo.h
	$(CC) $(CFLAGS) -o lzo_comp lzo_comp.c minilzo.c
	
config.h: conf.head conf.end Makefile
	cat conf.head > config.h
	echo \#define PARKER_HOME \"$(PARKER_HOME)\" >> config.h
	echo \#define HOSTLIST     \"$(PARKER_HOME)/etc/CollectList\" >> config.h
	echo \#define VALIDDAYS	$(VALIDDAYS) >>config.h

	cat conf.end >> config.h

flashdata: Makefile
	echo \#!/bin/sh > flashdata
	echo $(PARKER_HOME)/bin/collect >> flashdata
	echo $(PARKER_HOME)/bin/makestat >> flashdata
	chmod 755 flashdata


