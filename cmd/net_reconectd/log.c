#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdarg.h>
#include <syslog.h>
#include <stdio.h>

extern int log_syslog;

void log_info(const char *format, ...)
{
	char buffer[1024];
	va_list args;
	
	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	if (log_syslog) {
		syslog(LOG_INFO, "%s", buffer);
	} else {
		printf("\033[1;32;40m %s \033[0m \n",buffer);
	}
}

void log_err(const char *format, ...)
{
	char buffer[1024];
	va_list args;
	
	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	if (log_syslog) {
		syslog(LOG_INFO, "%s", buffer);
	} else {
		printf("\033[1;31;40m %s \033[0m\n",buffer);
	}
}
