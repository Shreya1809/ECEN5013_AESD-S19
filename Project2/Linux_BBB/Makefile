include sources.mk
WORKSPACE:=$(shell pwd)
WORKSPACE_FILES:=$(WORKSPACE)/*
INCLUDES:= -I./include/common -I./include/BeagleboneGreen #-I/usr/local/include 
TEST_INCLUDES:= -I./include/cmocka
OUTPUT_EXT:=.out
OUTPUT:= project2$(OUTPUT_EXT)
#CC:=arm-linux-gcc
CC:=arm-linux-gnueabihf-gcc

CFLAGS:= -O0 -g3 -Wall -Wno-unused-variable -Wno-unused-function -Wno-unused-but-set-variable
FLAGS:= $(CFLAGS) $(INCLUDES)
LIBS:= -L /usr/local/lib -lrt -lpthread -lm #-lmraa

LOCAL:=10.0.0.19
CLG:=128.138.189.80
IPADDR:=$(LOCAL)

.PHONY:all
all: build send test #doxygen rungdb
	 

.PHONY:rungdb
rungdb:
	gnome-terminal -- ssh root@$(IPADDR) "cd ~/home/Project2/config && ./startGDBServer.sh" &

.PHONY:build
build:
	echo $(WORKSPACE)
	$(CC) $(FLAGS) -o $(OUTPUT) $(SRCS) $(LIBS)

.PHONY:send
send:
	ssh root@$(IPADDR) "mkdir -p ~/home/Project2 ~/home/Project2/config ~/home/Project2/code ~/home/Project2/output" 
	#scp -r $(WORKSPACE_FILES) root@$(IPADDR):~/home/Project2/code/
	#scp -pr $(WORKSPACE)/src root@$(IPADDR):~/home/Project2/code/src
	#scp -pr $(WORKSPACE)/include root@$(IPADDR):~/home/Project2/code/include
	scp -pr $(WORKSPACE)/test root@$(IPADDR):~/home/Project2/code/test
	scp *$(OUTPUT_EXT) root@$(IPADDR):~/home/Project2/output/
	#scp -pr $(WORKSPACE)/config/* root@$(IPADDR):~/home/Project2/config/

.PHONY: test
test: buildtest runtest

.PHONY: buildtest
buildtest:
	$(CC) $(FLAGS) -DUART_TEST src/uart.c -o uartloopback_test.out -lpthread
	$(CC) $(FLAGS) test/ExternalClient.c -o client_test.out -lpthread
	gcc $(FLAGS) test/ExternalClient.c -o host_client_test.out -lpthread
	#$(CC) $(FLAGS) $(TEST_INCLUDES) test/Led_TEST/test.c -o led_test.out $(LIBS)
	#$(CC) $(FLAGS) $(TEST_INCLUDES) -DTEST_MODE -DHAVE_SIGNAL_H $(TEST_SRCS) test/Temp_TEST/test.c -o temp_test.out $(LIBS)
	#$(CC) $(FLAGS) $(TEST_INCLUDES) -DTEST_MODE -DHAVE_SIGNAL_H $(TEST_SRCS) test/Light_TEST/test.c -o light_test.out $(LIBS)
	#$(CC) $(FLAGS) $(TEST_INCLUDES) -DTEST_MODE -DHAVE_SIGNAL_H $(TEST_SRCS) test/Logger_TEST/test.c -o logger_test.out $(LIBS)
	@scp *.out root@$(IPADDR):~/home/Project2/output

.PHONY: runtest
runtest:
	@echo "\n********Executing test all test cases********\n"
	@ssh root@$(IPADDR) "cd ~/home/Project2/output; sync;  \
	echo -e "***Testing Logger***"; sync; ./logger_test.out; sync;"
	@echo "\n********All Tests executed********\n"

.PHONY: ssh
ssh:
	gnome-terminal -- ssh root@$(IPADDR)

.PHONY: doxygen
doxygen:
	#doxygen config/doxygen.config

.PHONY:clean
clean:
	rm -rf *.o *.out 
