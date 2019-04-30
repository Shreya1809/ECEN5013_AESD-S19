SRCDIR:=src/
SRCS:= $(SRCDIR)main.c\
	$(SRCDIR)comm_recv.c\
	$(SRCDIR)comm_send.c\
	$(SRCDIR)ext_clientHandler.c\
	$(SRCDIR)logger.c\
	$(SRCDIR)bbgled.c\
	$(SRCDIR)uart.c\
	$(SRCDIR)communicationPacket.c\
	$(SRCDIR)nodeInformation.c\
	
	
TEST_SRCS:= $(SRCDIR)cmocka.c\
	$(SRCDIR)logger.c\
	




				
				