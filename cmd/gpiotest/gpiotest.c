/*
 * Watchdog Driver Test Program
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/types.h>

int main(int argc, char *argv[])
{
    int state, fd;
    char *dev;    
    dev = argv[1];
    if(dev == NULL)
    {
	    printf("Please input name, for exmaple /dev/gpio_input_0.\n");
	    exit(-1);
    }

    fd = open(dev, O_RDONLY);
    if (fd < 0) {
	printf("open %s failed.\n", dev);
	exit(-1);
    }

    while(1) {
	    read(fd, &state, sizeof(state));
	    printf("read %s value is %d.\n", dev, state);
    }
}
