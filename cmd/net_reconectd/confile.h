

struct system_configs
{
	/* net port such as usb0 */
	char port[64]; 

	/* reconner count for repower or reboot */
	int reconnect_cnt;
	/* device linkdown time */
	int linkdown_time;

	/* enable repower net device*/
	int repower_en;
	/* repower count for reboot*/
	int repower_cnt;
	/* device power down time */
	int powerdown_time;
	/* max time of repower*/
	int max_repower_time;

	/* reboot enable*/
	int reboot_en;
	/* last reboot uptime + this reboot uptime > reboot_period */
	int reboot_period;

	/* time(second) between two pings, value 0 is disable ping function */
	int ping_delay;
	/* ping failed more then times, net is disconnect */
	int ping_faile_cnt;
	/* enable ping faile to restart network*/
	int ping_faile_en;

	/* enable compare packets to restart network */
	int cmp_packets_en;
	/* time(second) between two compare */
	int cmp_packets_delay;
	/* tx and rx add count between two compare */
	int rx_change_cnt;
	int tx_change_cnt;

	char cmd_powerdown[512];
	char cmd_powerup[512];
	char cmd_linkdown[512];
	char cmd_linkup[512];
	char cmd_ping[512];
};

struct system_configs* get_system_configs(char *configfile);

long read_packets_from_path(char *path);

int is_port_exist(char *port);

int read_int_from_file (char *pidfile);
int write_int_to_file (char *pidfile, int value);
