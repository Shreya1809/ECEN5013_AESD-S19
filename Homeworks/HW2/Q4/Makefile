TARGET = fileoperation
LIBS = -lm
#CC = ~/buildroot/output/host/usr/bin/arm-linux-gnueabihf-gcc
#CC =arm-buildroot-linux-uclibcgnueabihf-gcc
CC =/home/shreya1809/buildroot/output/host/usr/bin/arm-buildroot-linux-uclibcgnueabihf-gcc
CFLAGS = -g -Wall -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable --sysroot=/home/shreya1809/buildroot/output/staging

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)
