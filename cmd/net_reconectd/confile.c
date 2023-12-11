#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/file.h>

#include "confile.h"

static char port[] = "eth1";
static char cmd_powerdown[] = "/usr/bin/net_powerdown.sh";
static char cmd_powerup[] = "/usr/bin/net_powerup.sh";
static char cmd_linkdown[] = "/usr/bin/net_linkdown.sh";
static char cmd_linkup[] = "/usr/bin/net_linkup.sh";
static char cmd_ping[] = "ping www.baidu.com -c 1";
static int reboot_period = 24 * 60 * 60;

struct system_configs* get_system_configs(char *configfile)
{
	struct system_configs *new_syscfg = (struct system_configs *) malloc(sizeof(struct system_configs));

	strcpy(new_syscfg->port, port);

	new_syscfg->linkdown_time = 15;
	new_syscfg->reconnect_cnt = 3;

	new_syscfg->repower_en = 0;
	new_syscfg->repower_cnt = 3;
	new_syscfg->powerdown_time = 10;
	new_syscfg->max_repower_time = 120;
	new_syscfg->reboot_en = 0;

	new_syscfg->ping_faile_cnt = 2;
	new_syscfg->ping_faile_en = 0;
	new_syscfg->ping_delay = 0;

	new_syscfg->cmp_packets_en = 0;
	new_syscfg->cmp_packets_delay = 60;
	new_syscfg->rx_change_cnt = 11;
	new_syscfg->tx_change_cnt = 12;


	strcpy(new_syscfg->cmd_powerdown, cmd_powerdown);
	strcpy(new_syscfg->cmd_powerup, cmd_powerup);
	strcpy(new_syscfg->cmd_linkdown, cmd_linkdown);
	strcpy(new_syscfg->cmd_linkup, cmd_linkup);
	strcpy(new_syscfg->cmd_ping, cmd_ping);

	new_syscfg->reboot_period = reboot_period;


    	FILE *fp = fopen(configfile, "r");	
    	if (fp == NULL){		
       		printf("Failed to open file %s\n", configfile);	
		exit(1);	
    	} 

	char buf[1024] = {0};
	char *line;
	char *endofbuf = buf + 1023;
    	while(fgets(buf, sizeof(buf), fp) != NULL) {
		buf[strlen(buf) - 1] = '\0';
		line = buf;
		while(line < endofbuf && *line == ' ')
			line++;
		if(*line == '#')
			continue;
		if(!strncmp(line, "port", 4)) {
			line += 4;
			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				line++;
			 if(line == buf || *line != '=')
				 continue;
			 line++;

			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				 line++;
			 if(line == endofbuf || *line == '\0')
				 continue;

			 char *tmp = line;
			 while(tmp < endofbuf) { 
				 if(*tmp == ' ' || *tmp == '\t' || *tmp == '\0') {
					 *tmp = '\0';
					 break;
				 }
				tmp++;
			 }
			strcpy(new_syscfg->port, line);

		} else if(!strncmp(line, "reconnect_cnt", 13)) {
			line += 13;
			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				line++;
			 if(line == buf || *line != '=')
				 continue;
			 line++;

			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				 line++;
			 if(line == endofbuf || *line == '\0')
				 continue;
			new_syscfg->reconnect_cnt = strtol(line, NULL, 10);

		} else if(!strncmp(line, "linkdown_time", 13)) {
			line += 13;
			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				line++;
			 if(line == buf || *line != '=')
				 continue;
			 line++;

			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				 line++;
			 if(line == endofbuf || *line == '\0')
				 continue;
			new_syscfg->linkdown_time = strtol(line, NULL, 10);

		} else if(!strncmp(line, "repower_en", 10)) {
			line += 10;
			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				line++;
			 if(line == buf || *line != '=')
				 continue;
			 line++;

			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				 line++;
			 if(line == endofbuf || *line == '\0')
				 continue;
			new_syscfg->repower_en = strtol(line, NULL, 10);

		} else if(!strncmp(line, "repower_cnt", 11)) {
			line += 11;
			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				line++;
			 if(line == buf || *line != '=')
				 continue;
			 line++;

			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				 line++;
			 if(line == endofbuf || *line == '\0')
				 continue;
			new_syscfg->repower_cnt = strtol(line, NULL, 10);

		} else if(!strncmp(line, "powerdown_time", 14)) {
			line += 14;
			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				line++;
			 if(line == buf || *line != '=')
				 continue;
			 line++;

			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				 line++;
			 if(line == endofbuf || *line == '\0')
				 continue;
			new_syscfg->powerdown_time = strtol(line, NULL, 10);

		} else if(!strncmp(line, "max_repower_time", 16)) {
			line += 14;
			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				line++;
			 if(line == buf || *line != '=')
				 continue;
			 line++;

			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				 line++;
			 if(line == endofbuf || *line == '\0')
				 continue;
			new_syscfg->max_repower_time = strtol(line, NULL, 10);

		} else if(!strncmp(line, "reboot_en", 9)) {
			line += 9;
			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				line++;
			 if(line == buf || *line != '=')
				 continue;
			 line++;

			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				 line++;
			 if(line == endofbuf || *line == '\0')
				 continue;
			new_syscfg->reboot_en = strtol(line, NULL, 10);

		} else if(!strncmp(line, "reboot_period", 13)) {
			line += 13;
			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				line++;
			 if(line == buf || *line != '=')
				 continue;
			 line++;

			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				 line++;
			 if(line == endofbuf || *line == '\0')
				 continue;
			new_syscfg->reboot_period = strtol(line, NULL, 10);

		} else if(!strncmp(line, "ping_delay", 10)) {
			line += 10;
			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				line++;
			 if(line == buf || *line != '=')
				 continue;
			 line++;

			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				 line++;
			 if(line == endofbuf || *line == '\0')
				 continue;
			new_syscfg->ping_delay = strtol(line, NULL, 10);

		} else if(!strncmp(line, "ping_faile_cnt", 14)) {
			line += 14;
			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				line++;
			 if(line == buf || *line != '=')
				 continue;
			 line++;

			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				 line++;
			 if(line == endofbuf || *line == '\0')
				 continue;
			new_syscfg->ping_faile_cnt = strtol(line, NULL, 10);

		} else if(!strncmp(line, "ping_faile_en", 13)) {
			line += 13;
			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				line++;
			 if(line == buf || *line != '=')
				 continue;
			 line++;

			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				 line++;
			 if(line == endofbuf || *line == '\0')
				 continue;
			new_syscfg->ping_faile_en = strtol(line, NULL, 10);

		} else if(!strncmp(line, "cmp_packets_en", 14)) {
			line += 14;
			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				line++;
			 if(line == buf || *line != '=')
				 continue;
			 line++;

			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				 line++;
			 if(line == endofbuf || *line == '\0')
				 continue;
			new_syscfg->cmp_packets_en = strtol(line, NULL, 10);

		} else if(!strncmp(line, "cmp_packets_delay", 17)) {
			line += 17;
			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				line++;
			 if(line == buf || *line != '=')
				 continue;
			 line++;

			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				 line++;
			 if(line == endofbuf || *line == '\0')
				 continue;
			new_syscfg->cmp_packets_delay = strtol(line, NULL, 10);

		} else if(!strncmp(line, "rx_change_cnt", 13)) {
			line += 13;
			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				line++;
			 if(line == buf || *line != '=')
				 continue;
			 line++;

			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				 line++;
			 if(line == endofbuf || *line == '\0')
				 continue;
			new_syscfg->rx_change_cnt = strtol(line, NULL, 10);

		} else if(!strncmp(line, "tx_change_cnt", 13)) {
			line += 13;
			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				line++;
			 if(line == buf || *line != '=')
				 continue;
			 line++;

			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				 line++;
			 if(line == endofbuf || *line == '\0')
				 continue;
			new_syscfg->tx_change_cnt = strtol(line, NULL, 10);

		} else if(!strncmp(line, "cmd_powerdown", 13)) {
			line += 13;
			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				line++;
			 if(line == buf || *line != '=')
				 continue;
			 line++;

			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				 line++;
			 if(line == endofbuf || *line == '\0')
				 continue;
			strcpy(new_syscfg->cmd_powerdown, line);

		} else if(!strncmp(line, "cmd_powerup", 11)) {
			line += 11;
			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				line++;
			 if(line == buf || *line != '=')
				 continue;
			 line++;

			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				 line++;
			 if(line == endofbuf || *line == '\0')
				 continue;
			strcpy(new_syscfg->cmd_powerup, line);

		} else if(!strncmp(line, "cmd_linkdown", 12)) {
			line += 12;
			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				line++;
			 if(line == buf || *line != '=')
				 continue;
			 line++;

			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				 line++;
			 if(line == endofbuf || *line == '\0')
				 continue;
			strcpy(new_syscfg->cmd_linkdown, line);

		} else if(!strncmp(line, "cmd_linkup", 10)) {
			line += 10;
			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				line++;
			 if(line == buf || *line != '=')
				 continue;
			 line++;

			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				 line++;
			 if(line == endofbuf || *line == '\0')
				 continue;
			strcpy(new_syscfg->cmd_linkup, line);

		} else if(!strncmp(line, "cmd_ping", 8)) {
			line += 8;
			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				line++;
			 if(line == buf || *line != '=')
				 continue;
			 line++;

			 while(line < endofbuf && (*line == ' ' || *line == '\t')) 
				 line++;
			 if(line == endofbuf || *line == '\0')
				 continue;
			strcpy(new_syscfg->cmd_ping, line);

		}
	}

    	fclose(fp);



	return new_syscfg;
}


static int readfile(char *path, char *content, size_t size)
{
	int ret;
	FILE *f;
	f = fopen(path, "r");
	if (f == NULL)
		return -1;
	
	ret = fread(content, 1, size, f);
	fclose(f);

	return ret;
}

long read_packets_from_path(char *path)
{
	char buf[64];
	int ret;
	
	ret = readfile(path, buf, 64);
	if( ret <= 0)
		return 0;
	
	return strtol(buf, NULL, 10);
	


}

int is_port_exist(char *port)
{
	DIR *netdir;
	struct dirent *dent;
	int match = -1;
	netdir = opendir("/sys/class/net");
	if (netdir == NULL) {
		return 0;
	}

	while ((dent = readdir(netdir)) != NULL) {
		if (strcmp(dent->d_name, port) == 0) {
			match = 1;
			break;
		}
	}

	if(match != 1) {
		return 0;
	}

	closedir(netdir);
	return 1;
}
int read_int_from_file (char *pidfile)
{
	FILE *f;
	int pid;

	if (!(f=fopen(pidfile,"r")))
		return 0;
	fscanf(f,"%d", &pid);
	fclose(f);
	return pid;
}
int write_int_to_file (char *pidfile, int value)
{
 
       	FILE *f;
	int fd;
        
	if ( ((fd = open(pidfile, O_RDWR|O_CREAT|O_TRUNC, 0644)) == -1)
 			|| ((f = fdopen(fd, "r+")) == NULL) ) {
		fprintf(stderr, "Can't open or create %s.\n", pidfile);
		return 0;
	}   
  

	if (flock(fd, LOCK_EX|LOCK_NB) == -1) {
		fclose(f);
		printf("Can't lock.\n");
		return 0;
	}   
  

	if (!fprintf(f,"%d\n", value)) {
		printf("Can't write value, %s.\n", strerror(errno));
		close(fd);
		return 0;
	}   
      	fflush(f);
    

	if (flock(fd, LOCK_UN) == -1) {
	      		
		printf("Can't unlock pidfile %s, %s.\n", pidfile, strerror(errno));
		close(fd);
		return 0;
	}   
        	
	close(fd);
   
       	return 1;
}


