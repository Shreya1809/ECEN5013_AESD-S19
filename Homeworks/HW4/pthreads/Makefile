INCLUDE_DIRS = 
LIB_DIRS = 
CC =/home/shreya1809/buildroot/output/host/usr/bin/arm-linux-gcc

CDEFS= 
CFLAGS= -O3 -g $(INCLUDE_DIRS) $(CDEFS)
LIBS= -lpthread -lrt

PRODUCT=pthreads

HFILES=
CFILES= pthreads.c

SRCS= ${HFILES} ${CFILES}
OBJS= ${CFILES:.c=.o}

all:	${PRODUCT}

clean:
	-rm -f *.o *.NEW *~ *.d
	-rm -f ${PRODUCT} ${GARBAGE}
	-rm -f log.txt

pthreads:	pthreads.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ pthreads.o $(LIBS)

depend:

.c.o:
	$(CC) -MD $(CFLAGS) -c $<