TARGET=fltest_gpiotest

OBJS=gpiotest.o

CFLAGS+=-c -Wall -g

$(TARGET):$(OBJS)
	$(CC) $^ -o $@  

%.o:%.c                  
	$(CC) $^ $(CFLAGS) -o $@

clean:
	$(RM) *.o $(TARGET) -r
