/************************************************/
/*  log.c function version 4.0			*/
/*  this file is written for log                */
/*  call by search,submit       		*/
/*  improved from Noisy Parker                	*/
/*  author steff	2001/5/4                */
/************************************************/
#include "parker.h"

/* Are we to use syslog logging ? */

#ifdef SYSLOG

/* Logging through syslog call */
#include <syslog.h>
void Log(char *s1, char *s2, char *s3)
{
  char logline[MAX];

  sprintf(logline,s1,s2,s3);
  openlog("parker", LOG_PID | LOG_CONS, LOG_LOCAL7);
  syslog(LOG_INFO, logline);
  closelog();
}
#else

/* Logging to a file at $parker/log, add log time autometicly */
void Log(char *logfilename, char *logstring)
{
  char logline[MAX];
  char logfile[MAX],timestr[MAX];
  time_t now;
  FILE *log;
  time(&now);
  strcpy(timestr,ctime(&now));
  timestr[strlen(timestr)-1]='\0';
  sprintf(logline,"%s;from:%s;%s\n",timestr,getenv("REMOTE_ADDR"),logstring);
  sprintf(logfile,"%s/%s/%s",PARKER_HOME,LOGDIR,logfilename);
  if((log=fopen(logfile,"a+"))){
    fprintf(log,"%s",logline);
    fclose(log);
  }else
    printf("log file %s open fails.<br>\n",logfile);
}
/* do not add other string at logstring */
void onlyLog(char *logfilename, char *logstring)
{
  char logfile[MAX];
  FILE *log;
  sprintf(logfile,"%s/%s/%s",PARKER_HOME,LOGDIR,logfilename);
  if((log=fopen(logfile,"a+"))){
    fprintf(log,"%s\n",logstring);
    fclose(log);
  }else
    printf("log file %s open fails.<br>\n",logfile);
}
#endif
