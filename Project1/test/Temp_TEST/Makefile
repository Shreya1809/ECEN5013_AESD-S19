
CC:=arm-linux-gcc
CFLAGS:= -g3 -Wall -Wno-unused-variable -Wno-unused-function -Wno-unused-but-set-variable
LDFLAGS:= -pthread -lrt -lmraa -lm -lcmocka
INCLUDES:= -I ../../include/BeagleBone -I ../../include/cmocka -I ../../include/common

OUTPUT_EXT:=.out
OUTPUT:= temptest$(OUTPUT_EXT)

SRCS:= ../../src/myI2C.c ../../src/tempSensor.c 

.PHONY:build
build:
	$(CC) $(CFLAGS) $(INCLUDES) -o $(OUTPUT) test.c $(SRCS) $(LDFLAGS)
	scp $(OUTPUT) root@10.0.0.16:~/home/Project1/code/test/Temp_TEST

.PHONY:clean
clean:
	rm -f *.o $(OUTPUT_FILE)