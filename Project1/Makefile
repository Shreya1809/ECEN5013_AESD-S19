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

LOCAL:=10.0.0.16
CLG:=128.138.189.80
IPADDR:=$(CLG)

.PHONY:all
all: build test doxygen rungdb
	 

.PHONY:rungdb
rungdb:
	gnome-terminal -- ssh root@$(IPADDR) "cd ~/home/Project1/config && ./startGDBServer.sh" &

.PHONY:build
build:
	echo $(WORKSPACE)
	$(CC) $(FLAGS) -o $(OUTPUT) $(SRCS) $(LIBS)
	ssh root@$(IPADDR) "mkdir -p ~/home/Project1 ~/home/Project1/config ~/home/Project1/code ~/home/Project1/output" 
	#scp -r $(WORKSPACE_FILES) root@$(IPADDR):~/home/Project1/code/
	scp -pr $(WORKSPACE)/src root@$(IPADDR):~/home/Project1/code/src
	scp -pr $(WORKSPACE)/include root@$(IPADDR):~/home/Project1/code/include
	scp -pr $(WORKSPACE)/test root@$(IPADDR):~/home/Project1/code/test
	scp *$(OUTPUT_EXT) root@$(IPADDR):~/home/Project1/output/
	scp -pr $(WORKSPACE)/config/* root@$(IPADDR):~/home/Project1/config/

.PHONY: test
test: buildtest runtest

.PHONY: buildtest
buildtest:
	$(CC) $(FLAGS) test/Socket_TEST/client.c -o client_test.out -lpthread
	$(CC) $(FLAGS) $(TEST_INCLUDES) test/Led_TEST/test.c -o led_test.out $(LIBS)
	$(CC) $(FLAGS) $(TEST_INCLUDES) -DTEST_MODE -DHAVE_SIGNAL_H $(TEST_SRCS) test/Temp_TEST/test.c -o temp_test.out $(LIBS)
	$(CC) $(FLAGS) $(TEST_INCLUDES) -DTEST_MODE -DHAVE_SIGNAL_H $(TEST_SRCS) test/Light_TEST/test.c -o light_test.out $(LIBS)
	$(CC) $(FLAGS) $(TEST_INCLUDES) -DTEST_MODE -DHAVE_SIGNAL_H $(TEST_SRCS) test/Logger_TEST/test.c -o logger_test.out $(LIBS)
	@scp *.out root@$(IPADDR):~/home/Project1/output

.PHONY: runtest
runtest:
	@echo "\n********Executing test all test cases********\n"
	@ssh root@$(IPADDR) "cd ~/home/Project1/output; sync;  \
	echo -e "***Testing Light sensor***"; sync; ./light_test.out; sync; \
	echo -e "***Testing Temperature sensor***"; sync; ./temp_test.out; sync; \
	echo -e "***Testing Logger***"; sync; ./logger_test.out; sync;"
	@echo "\n********All Tests executed********\n"

.PHONY: ssh
ssh:
	gnome-terminal -- ssh root@$(IPADDR)

.PHONY: doxygen
doxygen:
	doxygen config/doxygen.config

.PHONY:clean
clean:
	rm -rf *.o *.out 
