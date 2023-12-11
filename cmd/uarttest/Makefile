TARGET=fltest_uarttest

OBJS=uarttest.o

CFLAGS+=-c -Wall -g
LDFLAGS+= -lpthread

$(TARGET):$(OBJS)
	$(CC) $^ $(LDFLAGS) -o $@  

%.o:%.c                  
	$(CC) $^ $(CFLAGS) -o $@

clean:
	$(RM) *.o $(TARGET) -r
