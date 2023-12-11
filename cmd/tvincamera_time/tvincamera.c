#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <asm/types.h>
#include <linux/videodev2.h>
#include <time.h>
#include <linux/fb.h>
#include <linux/input.h>
#include "drv_display.h"
#include "dragonboard_inc.h"
#include <pthread.h>
#include <sys/time.h>

#define CLEAR(x) memset (&(x), 0, sizeof (x))


/*time out*/
struct timeval tv1;
struct timeval tv2;
int fram_count=0;

struct buffer {
    void   *start;
    size_t length;
};

struct size {
	int width;
	int height;
};

struct options_t {
	char *devpath;
	int video_width;
	int video_height;
	int video_format;

	int debug;

        int channel;
	int fps;
};

//default paras for display
static struct options_t option = {
	.devpath      = "/dev/video0",      
	.video_width  = 640,
	.video_height = 480,
	.video_format = DISP_FORMAT_YUV422_I_YUYV,
	.debug        = 1,

        .channel= 0,
	.fps = 30,
};

static int fd;
static struct buffer *buffers = NULL;
static int nbuffers = 0;

static int read_frame(void)
{
	struct v4l2_buffer buf;
	memset(&buf, 0, sizeof(struct v4l2_buffer));
	buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	ioctl(fd, VIDIOC_DQBUF, &buf);
	ioctl(fd, VIDIOC_QBUF, &buf);
	return 1;
}

int video_mainloop(void)
{
	char dev_name[32];
	struct v4l2_format fmt;
	int i = 0;
	enum v4l2_buf_type type;
	struct v4l2_streamparm parms;
	struct v4l2_input inp;
	fd_set fds;
	struct v4l2_cropcap cropcap;
	struct v4l2_crop crop;

	strncpy(dev_name, option.devpath, 32);
	if ((fd = open(dev_name, O_RDWR | O_NONBLOCK, 0)) < 0) {
		db_error("can't open %s(%s)\n", dev_name, strerror(errno));
		goto open_err;
	}

        inp.index = 0;
        db_debug("inp.index: %d\n",inp.index);
        inp.type = V4L2_INPUT_TYPE_CAMERA;

        if (ioctl(fd, VIDIOC_ENUMINPUT, &inp) == -1) {
        	db_error("VIDIOC_S_INPUT error\n");
        	goto err;
        }

        if (ioctl(fd, VIDIOC_S_INPUT, &inp) == -1) {
        	db_error("VIDIOC_S_INPUT error\n");
        	goto err;
        }

	memset(&fmt, 0, sizeof(struct v4l2_format));
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        fmt.fmt.pix.width       = option.video_width;
        fmt.fmt.pix.height      = option.video_height;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
        fmt.fmt.pix.field       = V4L2_FIELD_NONE;
	db_debug("*********image source width = %d, height = %d********\n",fmt.fmt.pix.width, fmt.fmt.pix.height);
	if (ioctl(fd, VIDIOC_S_FMT, &fmt))
	{
		db_error("set image format failed\n");
	}

        parms.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        parms.parm.capture.timeperframe.numerator = 1;
        parms.parm.capture.timeperframe.denominator = option.fps;
        parms.parm.capture.capturemode = option.channel;
        if (ioctl(fd, VIDIOC_S_PARM, &parms) == -1) {
        	db_error("set frequence failed\n");
        	goto err;
        }

	usleep(100000);

	struct v4l2_requestbuffers req;
	CLEAR (req);
	req.count               = 4;
	req.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory              = V4L2_MEMORY_MMAP;
	ioctl(fd, VIDIOC_REQBUFS, &req);
	buffers = calloc(req.count, sizeof(struct buffer));

	for (nbuffers = 0; nbuffers < req.count; nbuffers++) {
        	struct v4l2_buffer buf;
	        memset(&buf, 0, sizeof(struct v4l2_buffer));
        	buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	        buf.memory = V4L2_MEMORY_MMAP;
        	buf.index  = nbuffers;
	        if (ioctl(fd, VIDIOC_QUERYBUF, &buf) == -1) {
        	    db_error("VIDIOC_QUERYBUF error\n");
	            goto buffer_rel;
        	}
	        buffers[nbuffers].start  = mmap(NULL, buf.length,
                PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        	buffers[nbuffers].length = buf.length;
	        if (buffers[nbuffers].start == MAP_FAILED) {
        	    db_error("mmap failed\n");
	            goto buffer_rel;
        	}
  	}

	for (i = 0; i < nbuffers; i++) {
		struct v4l2_buffer buf;

        	memset(&buf, 0, sizeof(struct v4l2_buffer));
	        buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index  = i;
		if (ioctl(fd, VIDIOC_QBUF, &buf) == -1) {
			db_error("VIDIOC_QBUF error\n");
			goto unmap;
		}
	}

    	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(fd, VIDIOC_STREAMON, &type) == -1) {
		db_error("VIDIOC_STREAMON error\n");
		goto stream_off;
	}
       
 
	while (fram_count < 500) {
		struct timeval tv;
       		int r;
	
		FD_ZERO(&fds);
		FD_SET(fd, &fds);

		/* timeout */
		tv.tv_sec  = 2;
		tv.tv_usec = 0;

		r = select(fd + 1, &fds, NULL, NULL, &tv);
		if (r == -1) {
			if (errno == EINTR) {
	    			continue;
			}	
			db_error("select error\n");
		}

		if (r == 0) {
			db_debug("select timeout\n");
			goto stream_off;
		}


		 read_frame();
                 fram_count ++;
		 if(fram_count == 1)
        		gettimeofday(&tv1,NULL);//begin time start
                 
	}

        float fps=0;
	fram_count--;
        gettimeofday(&tv2,NULL);
        fps=fram_count * 1000000.0 / (tv2.tv_sec*1000000 + tv2.tv_usec - tv1.tv_sec*1000000 + tv1.tv_usec);

        printf("fps=%f\n",fps);        

    	FD_ZERO(&fds);
stream_off:
	ioctl(fd, VIDIOC_STREAMOFF, &type);
unmap:
    for (i = 0; i < 4; i++) {
        munmap(buffers[i].start, buffers[i].length);
    }
buffer_rel:
    free(buffers);
err:   
    close(fd);
open_err:
    return -1;
}

static void usage(char *name)
{
	fprintf(stderr, "   Usage: %s [-d device] [-r resolution] [-c mode ] [-o fps][-h]\n", name);
	fprintf(stderr, "   -d: Video input device, default: /dev/video0\n");
	fprintf(stderr, "   -r: Resolution of video capture, something like: 640x480\n");
	fprintf(stderr, "   -c: video mode set\n");
	fprintf(stderr, "   -o: fps set\n");
	fprintf(stderr, "   -h: Print this message\n");
}

static void parse_opt(int argc, char **argv)
{
	int c;

	do {
		c = getopt(argc, argv, "d:r:f:p:s:o:c:l:t:vh");
		if (c == EOF)
			break;
		switch (c) {
		case 'd':
			option.devpath = optarg;
			break;
		case 'r':
			if (optarg)
				sscanf(optarg, "%dx%d\n",
					&option.video_width, &option.video_height);
			break;

		case 'o':
			if (optarg)
				sscanf(optarg, "%d\n", &option.fps);
			break;
                case 'c':
			if (optarg)
				sscanf(optarg, "%d\n", &option.channel);
			break;
		case 'v':
			option.debug = 1;
			break;
		case 'h':
		default:
			usage(argv[0]);
			exit(1);
			break;
		}
	} while (1);

	if (optind != argc) {
		usage(argv[0]);
		exit(1);
	}

	fprintf(stdout, "  devpth: %s\n", option.devpath);
	fprintf(stdout, "  width: %d\n", option.video_width);
	fprintf(stdout, "  height: %d\n", option.video_height);
	fprintf(stdout, "  format: %d\n", option.video_format);
}

int main(int argc, char *argv[])
{	
	parse_opt(argc, argv);
	db_debug("v40 tvin test v1 version 2016.1.20\n");
        video_mainloop();
        return 0;
}

