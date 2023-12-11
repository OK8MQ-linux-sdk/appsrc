TARGET=envcreat

OBJS=envcreat.o

CFLAGS+=-c -Wall -g

$(TARGET):$(OBJS)
	$(CC) $^ -o $@  

%.o:%.c                  
	$(CC) $^ $(CFLAGS) -o $@

clean:
	$(RM) *.o $(TARGET) -r
