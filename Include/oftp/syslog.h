#ifndef _SYS_LOG_
#define _SYS_LOG_


#define LOG_CRIT		0
#define LOG_INFO		0
#define LOG_DEBUG		0
#define LOG_ERR			0
#define LOG_WARNING		0
#define LOG_NOTICE		0


#define LOG_LOCAL0		0
#define LOG_LOCAL1		0
#define LOG_LOCAL2		0
#define LOG_LOCAL3		0
#define LOG_LOCAL4		0
#define LOG_LOCAL5		0
#define LOG_LOCAL6		0
#define LOG_LOCAL7		0

#define LOG_FTP			0

#define LOG_NDELAY		0


void  syslog(int , const char *fmt , ...);


#endif