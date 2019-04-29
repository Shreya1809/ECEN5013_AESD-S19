SRCDIR:=src/
SRCS:= $(SRCDIR)main.c\
	$(SRCDIR)comm_recv.c\
	$(SRCDIR)comm_send.c\
	$(SRCDIR)ext_clientHandler.c\
	$(SRCDIR)logger.c\
	$(SRCDIR)bbgled.c\
	$(SRCDIR)mytimer.c\
	$(SRCDIR)heartbeat.c\
	$(SRCDIR)mysignal.c\
	$(SRCDIR)uart.c
	
	
#TEST_SRCS:= $(SRCDIR)cmocka.c\
	$(SRCDIR)main.c\
	$(SRCDIR)bist.c\
	$(SRCDIR)light.c\
	$(SRCDIR)temp.c\
	$(SRCDIR)socket.c\
	$(SRCDIR)logger.c\
	$(SRCDIR)bbgled.c\
	$(SRCDIR)mytimer.c\
	$(SRCDIR)myI2C.c\
	$(SRCDIR)i2c.c\
	$(SRCDIR)tempSensor.c\
	$(SRCDIR)lightSensor.c\
	$(SRCDIR)heartbeat.c\
	$(SRCDIR)mysignal.c




				
				