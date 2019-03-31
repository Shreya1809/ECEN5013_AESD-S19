include sources.mk
WORKSPACE:=$(shell pwd)
WORKSPACE_FILES:=$(WORKSPACE)/*
INCLUDES:= -I./include/common -I./include/BeagleboneGreen -I/usr/local/include 
TEST_INCLUDES:= -I./include/cmocka
OUTPUT_EXT:=.out
OUTPUT:= project1$(OUTPUT_EXT)
CC:=arm-linux-gcc

CFLAGS:= -O0 -g3 -Wall -Wno-unused-variable -Wno-unused-function -Wno-unused-but-set-variable
FLAGS:= $(CFLAGS) $(INCLUDES)
LIBS:= -L /usr/local/lib -lrt -lpthread -lmraa -lm 

.PHONY:all
all: build rungdb 
	 

.PHONY:rungdb
rungdb:
	gnome-terminal -- ssh root@10.0.0.16 "cd ~/home/Project1/config && ./startGDBServer.sh" &

.PHONY:build
build:
	echo $(WORKSPACE)
	$(CC) $(FLAGS) -o $(OUTPUT) $(SRCS) $(LIBS)
	scp -r $(WORKSPACE_FILES) root@10.0.0.16:~/home/Project1/code/
	scp $(OUTPUT) root@10.0.0.16:~/home/Project1/output/
	scp -pr $(WORKSPACE)/config root@10.0.0.16:~/home/Project1/config/

.PHONY: test
test:
	$(CC) $(FLAGS) test/Socket_TEST/client.c -o client_test.out -lpthread
	$(CC) $(FLAGS) $(TEST_INCLUDES) test/Led_TEST/test.c -o led_test.out $(LIBS)
	$(CC) $(FLAGS) $(TEST_INCLUDES) -DTEST_MODE -DHAVE_SIGNAL_H $(TEST_SRCS) test/Temp_TEST/test.c -o temp_test.out $(LIBS)
	$(CC) $(FLAGS) $(TEST_INCLUDES) -DTEST_MODE -DHAVE_SIGNAL_H $(TEST_SRCS) test/Light_TEST/test.c -o light_test.out $(LIBS)
	$(CC) $(FLAGS) $(TEST_INCLUDES) -DTEST_MODE -DHAVE_SIGNAL_H $(TEST_SRCS) test/Logger_TEST/test.c -o logger_test.out $(LIBS)
	scp *.out root@10.0.0.16:~/home/Project1/output
	
.PHONY: doxygen
doxygen:
	cd doxygen && doxygen doxygen.config

.PHONY:clean
clean:
	rm -rf *.o *.out 
