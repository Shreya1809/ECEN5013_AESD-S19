include sources.mk
WORKSPACE:=$(shell pwd)
WORKSPACE_FILES:=$(WORKSPACE)/*
INCLUDES:= -I include/common -I include/BeagleboneGreen -I /usr/local/include
OUTPUT:= project1.out

CC:=arm-linux-gcc

CFLAGS:= -g3 -Wall -Wno-unused-variable -Wno-unused-function -Wno-unused-but-set-variable
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

.PHONY:clean
clean:
	rm -rf *.o *.out 
