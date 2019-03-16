include sources.mk
WORKSPACE:=$(shell pwd)
WORKSPACE_FILES:=$(WORKSPACE)/*
INCLUDES:= -I include/
OUTPUT:= project1.out

CC:=arm-linux-gcc

CFLAGS:= -g3 -Wall
FLAGS:= $(CFLAGS) $(INCLUDES)
LIBS:= -lrt -lpthread

.PHONY:all
all: rungdb build
	 

.PHONY:rungdb
rungdb:
	#gnome-terminal -- ssh root@10.0.0.16 "cd ~/home/Project1/output && gdbserver --multi localhost:4333" &
	gnome-terminal -- ssh root@10.0.0.16 "cd ~/home/Project1/config && ./startGDBServer.sh" &

.PHONY:build
build:
	echo $(WORKSPACE)
	$(CC) $(FLAGS) -o $(OUTPUT) $(SRCS) $(LIBS)
	scp -r $(WORKSPACE_FILES) root@10.0.0.16:~/home/Project1/code/
	scp $(OUTPUT) root@10.0.0.16:~/home/Project1/output/

.PHONY:clean
clean:
	rm -rf *.o *.out 
