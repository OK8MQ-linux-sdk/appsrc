
/*
 * Watchdog Driver Test Program
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <signal.h>
#include <syslog.h>

#include "pidfile.h"
#include "buildinfo.h"
#include "confile.h"
#include "log.h"

#define PidFile "/run/net_reconectd.pid"
#define VAR_UPTIME_FILE "/home/root/uptime"
#define PROC_UPTIME "/proc/uptime"

#define	NET_POWERDOWN 1
#define NET_LINKDOWN 2
#define NET_LINKUP   3
#define	NET_RECONNECT 4
#define	NET_REPOWER 5

struct network_infos {
	/* net state */
	int link_state;
	/* stored tx packets*/
	long tx_packets;
	/*stored rx packets */
	long rx_packets;
	/* stored uptime when end ping */
	int uptime_ping;
	/* stored uptime when end compare */
	int uptime_cmp_packets;
	/* already ping failed count */
	int ping_faile_cnt;
	/* already reconnect count */
	int reconnect_cnt;
	/* already repower count */
	int repower_cnt;
	/* link down uptime */
	int uptime_faile;
};

int log_syslog = 1;
char    *ConfFile = "/etc/net_reconectd.conf";

struct network_infos net_infos;
struct system_configs *net_config;
int last_reboot_uptime;

/* read /proc/uptime, return uptimes */
static int get_uptime_proc()
{
	return read_int_from_file(PROC_UPTIME);
}
/* read last reboot uptime in file */
static int get_uptime_file()
{
	return read_int_from_file(VAR_UPTIME_FILE);
}
/* write uptime to file */
static int write_uptime_file(int uptime)
{
	return write_int_to_file(VAR_UPTIME_FILE, uptime);
}
/* every 10 min write uptime to file */
static void update_uptime_file()
{
	if(get_uptime_proc() - get_uptime_file() > 600)
		write_uptime_file(get_uptime_proc());	
}
/* check reboot period is enough */
static int reboot_period_passed()
{
	log_info("last reboot uptime %d, proc uptime %d, configs reboot_period %d\n", last_reboot_uptime, get_uptime_proc(), net_config->reboot_period);
	return (last_reboot_uptime + get_uptime_proc()) > net_config->reboot_period;
}
/* run ping command */
static int start_ping()
{
	return system(net_config->cmd_ping);
}
/* run powerdown command */
static int powerdown_network()
{
	return system(net_config->cmd_powerdown);
}
/* run powerup command */
static int powerup_network()
{
	return system(net_config->cmd_powerup);
}
/* run linkdown command */
static int linkdown_network()
{
	return system(net_config->cmd_linkdown);
}

/* run linkup command */
static int linkup_network()
{
	return system(net_config->cmd_linkup);
}

/* reboot */
static void reboot_system()
{
	system("reboot");
	while(1) {
		log_info("waiting for reboot complete\n");
		sleep(1);
	}
}


void die(int sig)

{
	log_info("test: exit running.\n");
	remove_pid(PidFile);
	free(net_config);

	exit(0);
}

void show_info()
{
	char *build_time = BUILD_DATE;
	log_info("build at %s\n", build_time);
}

void dump_config()
{
	log_info("port: %s\n",  net_config->port);

	log_info("reconnect count: %d\n", net_config->reconnect_cnt);
	log_info("linkdown_time: %d\n",  net_config->linkdown_time);

	log_info("repower_en: %d \n",   net_config->repower_en);
	log_info("repower_cnt: %d\n",  net_config->repower_cnt);
	log_info("powerdown_time: %d\n",  net_config->powerdown_time);
	log_info("max_repower_time: %d\n",  net_config->max_repower_time);

	log_info("reboot_en: %d\n",  net_config->reboot_en);
	log_info("reboot_period: %d\n",  net_config->reboot_period);

	log_info("ping_delay: %d\n",  net_config->ping_delay);
	log_info("ping_faile_cnt: %d\n",  net_config->ping_faile_cnt);
	log_info("ping_faile_en: %d\n",  net_config->ping_faile_en);

	log_info("cmp_packets_en: %d\n",  net_config->cmp_packets_en);
	log_info("cmp_packets_delay: %d\n",  net_config->cmp_packets_delay);
	log_info("rx_change_cnt: %d\n",  net_config->rx_change_cnt);
	log_info("tx_change_cnt: %d\n",  net_config->tx_change_cnt);

	log_info("cmd_powerdown: %s\n",  net_config->cmd_powerdown);
	log_info("cmd_powerup: %s\n",  net_config->cmd_powerup);
	log_info("cmd_linkdown: %s\n",  net_config->cmd_linkdown);
	log_info("cmd_linkup: %s\n",  net_config->cmd_linkup);

}

int usage()
{
	        fprintf(stderr, "usage: net_reconectd [-d] [-f conffile]\n \
				-d use printf for debug \n");
		        exit(1);
}

int main(int argc, char **argv)
{
	int ch;
	while ((ch = getopt(argc, argv, "f:d")) != EOF)
		switch((char)ch) {
			 case 'f':
				  ConfFile = optarg;
				 break;
			 case 'd':
				 log_syslog = 0;
				 break;
			 default:
				 usage();
		}

	/* show build date*/
	show_info();
	/* check is Already running ? */
	pid_t ppid = getpid();
	if (!check_pid(PidFile))
	{
		 if (!write_pid(PidFile))
		 {
			 if (getpid() != ppid)
				 kill (ppid, SIGTERM);
			 exit(1);
		 }
	} else {
		fputs("test: Already running.\n", stderr);
		if (getpid() != ppid)
			kill (ppid, SIGTERM);
		exit(1);
	}
	
	/* quit signal */
	signal(SIGTERM, die);
	signal(SIGINT, die);
	/* get configs*/
	net_config = get_system_configs(ConfFile);
	dump_config();
	char tx_packets_path[1024] = {0};
	char rx_packets_path[1024] = {0};

	/* set tx_packets_path and rx_packets_path */
	sprintf(tx_packets_path, "/sys/class/net/%s/statistics/tx_packets", net_config->port);
	sprintf(rx_packets_path, "/sys/class/net/%s/statistics/rx_packets", net_config->port);
	
	/* get last reboot's uptime and write uptime of now to file */
	last_reboot_uptime =  get_uptime_file();
	log_info("last reboot uptime = %d \n", last_reboot_uptime);
	write_uptime_file(get_uptime_proc());

	/* if port not exist NET_POWERDOWN else link down*/
	if(!is_port_exist(net_config->port))
		net_infos.link_state = NET_POWERDOWN;
	else 
		net_infos.link_state = NET_LINKDOWN;

	net_infos.uptime_faile = get_uptime_proc();

	while(1) {
		
		/* update uptime to file every 10 min*/
		update_uptime_file();

		/* link state is repower, power down the module */
		if(net_infos.link_state == NET_REPOWER) {
			net_infos.link_state = NET_POWERDOWN;
			log_info("uptime:%d --> ", get_uptime_proc());
			log_info("power down the device\n");
			powerdown_network();
			sleep(net_config->powerdown_time);
		}
		
		/* link state is power down, power up the module */
		if(net_infos.link_state == NET_POWERDOWN) {
			net_infos.link_state = NET_LINKDOWN;
			log_info("uptime:%d --> ", get_uptime_proc());
			log_info("power up the device\n");
			powerup_network();
		}
		

		/* wating for net port appear */
		if(!is_port_exist(net_config->port)) {
			if(get_uptime_proc() - net_infos.uptime_faile > net_config->max_repower_time) {
				log_err("uptime:%d --> ", get_uptime_proc());
				log_err("timeout for repower \n");
				net_infos.reconnect_cnt = net_config->reconnect_cnt;
				goto failed;
			}
			log_info("uptime:%d --> ", get_uptime_proc());
			log_info("can not find port %s \n", net_config->port);
			sleep(3);
			continue;
		}
		
		/* link up when link down*/
		if(net_infos.link_state == NET_LINKDOWN) {
			if( linkup_network() ) {
				log_err("uptime:%d --> ", get_uptime_proc());
				log_err("link up the device failed\n");
				goto failed;
			}
			net_infos.link_state = NET_LINKUP;
			log_info("uptime:%d --> ", get_uptime_proc());
			log_info("link up the device\n");
                        /* update stored compare time */
                        net_infos.uptime_cmp_packets = get_uptime_proc();
			/* update stored ping time */
			net_infos.uptime_ping = get_uptime_proc();
			continue;
		}

		/* link down when need reconnect */
		if(net_infos.link_state == NET_RECONNECT) {
			net_infos.link_state = NET_LINKDOWN;
			linkdown_network();
			log_info("uptime:%d --> ", get_uptime_proc());
			log_info("link down the device\n");
			sleep(net_config->linkdown_time);
			continue;
		}

		if( net_config->cmp_packets_en && 
				(get_uptime_proc()- net_infos.uptime_cmp_packets) > net_config->cmp_packets_delay )
		{
			/* update stored compare time */
			net_infos.uptime_cmp_packets = get_uptime_proc();

			/* tx and rx packets do not change*/
			if((read_packets_from_path(rx_packets_path) - net_infos.rx_packets <= net_config->rx_change_cnt) 
			    || (read_packets_from_path(tx_packets_path) - net_infos.tx_packets <= net_config->tx_change_cnt)) {
				log_err("uptime:%d --> ", get_uptime_proc());
				log_err("tx or rx packets is not changed, restart the network.\n");
				goto failed;
			}

			net_infos.tx_packets = read_packets_from_path(tx_packets_path);
			net_infos.rx_packets = read_packets_from_path(rx_packets_path);

			log_info("uptime:%d --> ", get_uptime_proc());
			log_info("tx or rx packets comapre test is passed.\n");

		}
		
		if( net_config->ping_delay != 0 
				&& (get_uptime_proc() - net_infos.uptime_ping) > net_config->ping_delay) {
			if(start_ping() == 0) {
				if(net_config->ping_faile_en) {
					net_infos.ping_faile_cnt = 0;
					net_infos.reconnect_cnt = 0;
					net_infos.repower_cnt = 0;
					log_info("uptime:%d --> ", get_uptime_proc());
					log_info("ping test is passed.\n");
				}
			 } else if(net_config->ping_faile_en) {
				net_infos.ping_faile_cnt++;
				log_info("uptime:%d --> ", get_uptime_proc());
				log_info("ping test failed, count = %d\n", net_infos.ping_faile_cnt);
				 if(net_infos.ping_faile_cnt >= net_config->ping_faile_cnt) {
					log_err("uptime:%d --> ", get_uptime_proc());
					log_err("ping test failed, restart the network\n");
				 	goto failed;
				 }	
			 }
			net_infos.uptime_ping = get_uptime_proc();
		}

		sleep(1);
		continue;
failed:
		net_infos.uptime_faile = get_uptime_proc();
		if(net_infos.reconnect_cnt < net_config->reconnect_cnt) {
			net_infos.link_state = NET_RECONNECT;
			net_infos.reconnect_cnt++;
			log_err("uptime:%d --> ", get_uptime_proc());
			log_err("network goto reconnect state, reconnect times = %d \n", net_infos.reconnect_cnt);
		} else if(net_infos.repower_cnt < net_config->repower_cnt || net_config->reboot_en == 0 || !reboot_period_passed()) {
			net_infos.link_state = NET_REPOWER;
			net_infos.reconnect_cnt = 0;
			net_infos.repower_cnt++;
			log_err("uptime:%d --> ", get_uptime_proc());
			log_err("network goto repower state, repower times = %d \n", net_infos.repower_cnt);
		} else {
			log_err("uptime:%d --> ", get_uptime_proc());
			log_err("system will reboot because the network Abnormal work.\n");
			reboot_system();
		}	
	}
		
}
