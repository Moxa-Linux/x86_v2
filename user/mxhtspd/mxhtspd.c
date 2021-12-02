#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>
#include <stdarg.h>
#include <syslog.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "mxhtsp.h"
#include "mxhtspd.h"

/* Jared, the control should be moved the global variable for accessing safely in another sub-function */
/* default control setting */
static daemon_ctrl control= { 
	.time_btn_long = TIME_BTN_LONG_PRESSED, 
	.time_chk_part = TIME_PARTITION_CHECK,
	.force_chk_part = 0,
	.log_facility = -1,
	.daemonize = 0,
	.verbose = 0, 
	.exit = 0
};

static daemon_ctrl *ctrl;	/* A pointer for accessing control */

static const char *dir="/etc/mxhtspd/";


static void msleep(unsigned long msec)
{
	struct timeval	time;
	time.tv_sec = msec / 1000;
	time.tv_usec = (msec % 1000) * 1000;
	select(1, NULL, NULL, NULL, &time);
}

static void mprintf(const char *fmt, ... )
{
	char buf[BUFSIZ];
	va_list ap;

	va_start(ap, fmt) ;
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);

	if (ctrl->log_facility >= 0) 
		syslog(LOG_NOTICE,"%s", buf); 
	else 
		printf("mxhtspd: %s", buf);
}

static void execute_script(char *cmd, int param)
{
	char name[50];

	d("%s %d\n", cmd, param);
	sprintf(name, "%s%s %d &", dir, cmd, param);
	d("%s\n", name);
	system(name);
}

static void handle_disk_plugged(int fd, int n, htsp_info *info) 
{
	int ret;
	ret = mxhtsp_is_disk_plugged(fd, n); 

	if ((ret == 1) && (info[n-1].disk_plugged == 0)) {	

		mprintf("Disk %d is plugged\n", n);
		if (ctrl->verbose) mprintf("call action-disk-plugged %d\n", n);
		execute_script("action-disk-plugged", n);
		ctrl->force_chk_part = 1;
		info[n-1].first_insert = 1;
		sleep (1);
	} 
	else if ((ret == 0) && (info[n-1].disk_plugged == 1)) {

		mprintf("Disk %d is unplugged\n", n);
		if (ctrl->verbose) mprintf("call action-disk-plugged %d\n", n);
		/* 
		 * Note: partition should be umounted before unplugged, and 
		 * it's advicable to umount all partition in action-disk-unplugged script.
		 * Here is just a prevention for operation mistake
		 */
		execute_script("action-disk-unplugged", n);
		ctrl->force_chk_part = 1;
	} 

	info[n-1].disk_plugged = ret;
}

static void handle_disk_busy(int fd, int n, htsp_info *info) 
{
	if (info[n-1].led_warn)
		return;

	if ( mxhtsp_is_disk_busy(fd, n) == 1) { /* busy */
#if ( CONFIG_PRODUCT == V2616A )
		mxhtsp_set_led(fd, 1, 1);
#else
		mxhtsp_set_led(fd, n, 1);
#endif
		info[n-1].led_on = 1;
	} 
	else if (info[n-1].led_on) { /* not busy */
#if ( CONFIG_PRODUCT == V2616A )
		mxhtsp_set_led(fd, 1, 0);
#else
		mxhtsp_set_led(fd, n, 0);
#endif
		info[n-1].led_on = 0;
	}
}

static int handle_button_pressed(int fd, int n, htsp_info *info) 
{
	int now_press;
	//int *ta = &info[n-1].btn_time_accum;
	int *btn_event_start = &info[n-1].btn_event_start;
	int *btn_last_pressed = &info[n-1].btn_last_pressed;

	now_press = mxhtsp_is_button_pressed(fd, n); 

	if (now_press < 0) {
		mprintf("detect button pressed error!");
		return -1;
	}

	if (now_press == 1 && (*btn_last_pressed == 0)) { 
		*btn_event_start = 1;
		*btn_last_pressed = 1;
	} 
	else if (now_press == 0) {
		if (*btn_event_start && info[n-1].first_insert == 0) {
			mprintf("Button %d is pressed\n", n);
			if (ctrl->verbose) 
				mprintf("call action-btn-pressed %d\n", n);
#if ( CONFIG_PRODUCT == V2616A )
			execute_script("action-btn-pressed", 1);
#else
			execute_script("action-btn-pressed", n);
#endif
			*btn_event_start = 0;
		}
		*btn_last_pressed = 0;
		info[n-1].first_insert = 0;
	} 

	return 0;
}

void sig_handler(int signo)
{
	ctrl->exit = 1;
}

static int clist_check_insert(clist **cl, clist *new)
{
	clist *c;

	if (*cl == NULL) {
		*cl = new;
		return 0; 
	} 

	for (c = *cl; c->next; c = c->next) {
		if (strcmp(c->mpoint, new->mpoint) == 0) {
			mprintf("Error! %s is dulplicated in configuration file\n");
			exit(1);
		} 
	}
	c->next = new;
	
	d("add mpoint=%s percent=%d\n", new->mpoint, new->percent); 
	return 0;
}

static int read_conf(char *path, clist **cl) 
{
	FILE *fp;
	char buf[BUFSIZ];
	char *delim=" \n";
	char *token = NULL;
	char *mpoint;
	int percent;
	int n, m;
	clist *new = NULL;

	if (!(fp = fopen(path, "r"))) 
		perror("read_conf");

	while (fgets(buf, BUFSIZ, fp)) {

		/* mount point */
		if (!(token = strtok(buf, delim))) 
			goto err;
		if (strncmp(token, "#", 1) == 0) 
			continue;
		mpoint = strdup(token);
		/* check if syntax correct */
		if (((sscanf(mpoint, "/media/disk%dp%d", &n, &m) != 2)) 
				|| ((n != 1) && (n != 2)) 
				|| (m < 0)) {
			mprintf("prase configuration error => incorrect mount point %s\n", mpoint);
			exit(1);
		}

		/* percentage */
		if (!(token = strtok(NULL, delim))) 
			goto err;
		percent = atoi(token);
		if ((percent > 100) || (percent < 0)) {
			mprintf("parse configuration error => incorrect usage limit %d\n", percent);
			exit(1);
		}

		new = malloc(sizeof(clist));
		memset(new, 0, sizeof(clist));
		new->mpoint = mpoint;
		new->percent = percent;
		if (clist_check_insert(cl, new)) 
			free(new);

	}

	fclose(fp);
	return 0;

err:
	fclose(fp);
	mprintf("parsing %s fail, content=%s\n", path, buf); 
	return -1;
}

static const char *action_part_script="action-part-over-usage";

/* 
 * Note: this function is called per second
 * For a paration in /media/diskn, daemon will light the warning led n 
 * in following situations:
 * 1. partition doesn't exist
 * 2. partition is over its usage limit
 */
static void  check_part_usage(int fd, clist *cl, htsp_info *info) 
{

	static int cnt_chk_part;
	static int on;
	int percent;
	int n, p;
	int i;
	char *script;
	clist *c;

	d("cnt_chk_part=%d force_chk_part=%d time_chk_part=%d\n", cnt_chk_part, ctrl->force_chk_part, ctrl->time_chk_part);

	/* when the force_chk_part is set, we wait 10s and check the partition usage */
	if (ctrl->force_chk_part) {
		cnt_chk_part = TIME_DELAY_CHECK_USAGE;
		ctrl->force_chk_part = 0;
		d("turn force_chk_part=%d\n", ctrl->force_chk_part);
	}

	/* check partition in time_chk_part seconds */
	if (++cnt_chk_part > ctrl->time_chk_part) {
		d("check partition\n");

		for (c = cl; c ; c = c->next) {
			percent = mxhtsp_check_partition_usage(c->mpoint); 
			sscanf(c->mpoint,"/media/disk%dp%d", &n, &p); 

			if (percent == -1) {
				mprintf("%s doesn't exist\n", c->mpoint); 
				info[n-1].led_warn = 1;
			}
			else if (percent > c->percent) 
			{
				sscanf(c->mpoint,"/media/disk%dp%d", &n, &p); 
				mprintf("%s is over its usage limit (%d%% > %d%%)\n", 
						c->mpoint, percent, c->percent);
				if (ctrl->verbose) 
					mprintf("call %s %s\n", action_part_script, c->mpoint);
				script = malloc(strlen(dir) + strlen(action_part_script) + strlen(c->mpoint) + 4);
				sprintf(script, "%s%s %s &", dir, action_part_script, c->mpoint);
				system(script);
				free(script);
				info[n-1].led_warn = 1;
			} 
			else {
				info[n-1].led_warn = 0;
			}
		}
		cnt_chk_part = 0;
	}

#if ( CONFIG_PRODUCT == V2616A )
	/* Jared, 1-13-2014, V2616A only has one LED */
	/* When one of the disk usage full, we should turn the LED on */
	i=1;

	/* if in warning state */
	if (info[i].led_warn) {
		d("set led on");
		on=1;
		mxhtsp_set_led(fd, i, on);
		info[i-1].led_on = 1;
		info[i-1].led_last_warn = 1;
	} 
	/* if no warning anymore */
	else if (info[i].led_last_warn) {
		d("set led off");
		on=0;
		mxhtsp_set_led(fd, i, 0);
	}
#else
	/* light warning led per second*/
	on = (on + 1) % 2;
	for (i = 1; i <= 2 ; i++) {
		/* if in warning state */
		if (info[i-1].led_warn) {
			d("set led %d %d", i, on);
			mxhtsp_set_led(fd, i, on);
			info[i-1].led_on = 1;
			info[i-1].led_last_warn = 1;
		} 
		/* if no warning anymore */
		else if (info[i-1].led_last_warn) {
			d("set led %d 0", i);
			mxhtsp_set_led(fd, i, 0);
		}
	}
#endif
}

static void show_clist_info(clist **cl) {
	clist *c;
	int ret; 

	mprintf("Check the following partitions usage:\n");
	for (c = *cl; c; c = c->next) {
		ret = mxhtsp_check_partition_usage(c->mpoint);
		if (ret < 0)
			mprintf("    WARNING! %s doesn't exist\n", c->mpoint);
		else 
			mprintf("    %s - current usage is %d%%\n", c->mpoint, ret);
	}
}
static void usage() {
	printf("Usage: mxhtspd [options] \n");
	printf("\t-i interval in seconds to check partition usage (defulat 60 secs) \n");
	printf("\t-l facility_num log daemon's message with LOCAL[facility_num]\n");
	printf("\t-v run in verbose mode \n");
	printf("\t-h print usage\n");
	exit(1);
}

static void parseopt(int argc,char **argv) {
	char opt;
	int num;
	int facility[8] = {LOG_LOCAL0, LOG_LOCAL1, LOG_LOCAL2, LOG_LOCAL3, 
		LOG_LOCAL4, LOG_LOCAL5, LOG_LOCAL6, LOG_LOCAL7};

	while ((opt = getopt(argc, argv, "t:i:l:dvh")) != -1) {
		switch(opt) {
			case 't':
				num = atoi(optarg);
				if (num <= 0) {
					mprintf("can't set -t  %d", ctrl->time_btn_long);
					exit(1);
				}
				ctrl->time_btn_long = num;
				break;

			case 'i':
				num = atoi(optarg);
				if (num <= 0) {
					mprintf("can't set -i %d", ctrl->time_chk_part);
					exit(1);
				}
				ctrl->time_chk_part = num;
				break;

			case 'l':
				num = atoi(optarg);
				if ((num > 7) && (num < 0)) {
					mprintf("set a wrong log facility %d\n", num);
					exit(1);
				} 
				openlog ("mxhtspd", LOG_CONS | LOG_PID | LOG_NDELAY, facility[num]); 
				syslog(LOG_NOTICE,"Starting mxthspd daemon..."); 
				d("start with local %d\n", num);
				ctrl->log_facility = num;
				break;

			case 'd':
				ctrl->daemonize = 1;
				break;

			case 'v':
				ctrl->verbose = 1;
				break;

			case 'h':
				usage();
				break;

			case '?':
				usage();
		}
	}
}

static int create_pid_file() 
{
	FILE *fp;
	pid_t pid = getpid();

	fp = fopen(PID_FILE,"w");

	if (!fp) {
		mprintf("can't open pid file\n");
		return -1;
	}

	if (fprintf(fp, "%d\n", (int)pid) <=0 ) {
		return -1;
	}

	fclose(fp);
	return 0;
}

void remove_pid_file() 
{
	if (unlink(PID_FILE))
		mprintf("can't remove pid file");
}

static void daemonize() 
{
	pid_t pid, sid;

	/* already a daemon */
	if (getppid() == 1) 
		return;

	pid = fork();

	/* error */
	if (pid < 0) {
		exit(1);
	}

	/* fork off parent */
	if (pid > 0) {
		exit(0);
	}

	/* now execute as child process */
	umask(0);

	/* get a unique session id */
	sid = setsid();
	if (sid < 0) {
		exit(1);
	}

	/* change work directory to /, prevent to lock a directory */
	if ((chdir("/")) < 0) {
		exit(1);
	}

	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
}

int main(int argc, char *argv[]) {
	int ret;
	int fd;
	int i;
	htsp_info info[2] = {};
	clist *cl = NULL;

	ctrl = &control;
	parseopt(argc, argv);

	/* daemonzie */
	if (ctrl->daemonize)
		daemonize();

	/* read config */
	ret = read_conf("/etc/mxhtspd/mxhtspd.conf", &cl);
	if (ret == -1) 
		return -1;
	show_clist_info(&cl);

	fd = mxhtsp_open();
	if (fd < 0) {
		mprintf("can't open device\n");
		exit(1);
	}

	/* signal handler */
	signal(SIGTERM, sig_handler);
	signal(SIGINT, sig_handler);
	signal(SIGKILL, sig_handler);

	mprintf("Set the following parameter:\n");
//	mprintf("    button long pressed time=%ds\n",ctrl->time_btn_long);
	mprintf("    partition checking interval=%ds\n", ctrl->time_chk_part);
	
	create_pid_file();

	/*
	 * Main loop
	 */
	int cnt = 0;

	while (1) {

		if (ctrl->exit) 
			break;

		for (i = 1; i <= 2; i++) {
			handle_disk_plugged(fd, i, info);
			handle_disk_busy(fd, i, info);
			handle_button_pressed(fd, i, info);
		}

		/* each second, call check_part_usage */
		if (++cnt*TIME_SLEEP_MS > 1000) {
			check_part_usage(fd, cl, info);
			cnt = 0;
		}


		msleep(TIME_SLEEP_MS);
	}

	mprintf("Stopping...\n");
	remove_pid_file();
	closelog();
	mxhtsp_set_led(fd, 1, 0);
#if ( CONFIG_PRODUCT == V2416A )
	mxhtsp_set_led(fd, 2, 0);
#endif
	close(fd);
	return 0;
}
