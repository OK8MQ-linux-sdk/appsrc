TARGET=net_reconectd

OBJS=net_reconectd.o pidfile.o confile.o log.o
CFLAGS+=-c -Wall -g
all: head $(TARGET) 

$(TARGET):$(OBJS)
	$(CC) $^ -o $@  

%.o:%.c                  
	$(CC) $^ $(CFLAGS) -o $@

clean:
	$(RM) *.o $(TARGET) buildinfo.h -r
head:
	@echo "#define BUILD_DATE  \"` date  --iso-8601=seconds` \"" > buildinfo.h
