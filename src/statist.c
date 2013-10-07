/************************************************/
/*  statist.c cgi version 4.2			*/
/*  this file is written for FTP site 		*/
/*  statistic.              			*/
/*  improved from Noisy Parker                	*/
/*  author steff	2002/3/14               */
/************************************************/
#define CGI_VERSION   "2.4"

#include "parker.h"
#include "language.h"
int main(void)
{
  char *query;
  char genbuf[MAX],dnsname[MAX],ipstring[MAX],timestr[MAX];
  unsigned long files=0,rms=0,mp3=0,winfiles=0,linuxfiles=0;
  FILE *statfile;
  char *User_Language;

  alarm(CGIALARM);
    User_Language=getenv(ACCEPT_LANGUAGE);
  if(User_Language && *User_Language && strstr(User_Language,"zh-cn"))
  	{
  		UserLanguage=&ChineseDef;
  	}
  else
  		UserLanguage=&EnglishDef;

  query=getenv(QUERYSTRING);
  puts("Content-type:text/html\n\n");
  printf("<html><head><TITLE>%s</TITLE></head>\n",UserLanguage->HFTPList);
  puts("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=gb2312\">\n");
  puts("<BODY BGCOLOR=#ffccaa  align=left>\n");
  printf("<div align=center><H1>%s</H1></div>",UserLanguage->HFTPList);

  if(query && *query && *query!='.' )
    sprintf(genbuf,"%s/%s/%s",PARKER_HOME,HOSTDATADIR,query);
  else
    sprintf(genbuf,"%s/%s/%s",PARKER_HOME,HOSTDATADIR,"files");
  if(!(statfile=fopen(genbuf,"r"))){
#ifdef DEBUG
    printf("repuest file: %s\n",genbuf); 
#endif
    printf("No Data for this request.\n");
  }else{    
    puts("<table border=3 cellspacing=4 align=center >\n");
    printf("<tr align='center' ><td nowrap>%s</td><td>IP</td>",UserLanguage->HSite);
    printf("<td nowrap><a href='statist?files'>%s</a></td>",UserLanguage->HArchiveNum);
    printf("<td nowrap><a href='statist?rms'>%s</a></td>",UserLanguage->HFilmNum);
    printf("<td nowrap><a href='statist?mp3'>%s</a></td>",UserLanguage->HMusicNum);
    printf("<td nowrap><a href='statist?winfiles'>%s</a></td>",UserLanguage->HWinSoftNum);
    printf("<td nowrap><a href='statist?linuxfiles'>%s</a></td>\n",UserLanguage->HLinuxSoftNum);
    printf("<td nowrap>%s</td><td>站点简介</td></tr>\n",UserLanguage->HUpdataTime);
    while( fscanf(statfile,"%s %s %lu %lu %lu %lu %lu",dnsname,ipstring,&files,&rms,&mp3,&winfiles,&linuxfiles)!=EOF ){
      getfiletime(ipstring, timestr,UserLanguage);
      printf("<tr><td ><a href='ftp://%s'>%s</a></td><td ><a href='ftp://%s'>%s</a></td><td>%lu</td><td>%lu</td><td>%lu</td><td>%lu</td><td>%lu</td><td nowrap>%s</td><td><a href='siteinfo?%s'>简介</a></td></tr>\n",dnsname,dnsname,ipstring,ipstring,files,rms,mp3,winfiles,linuxfiles,timestr,ipstring);
    }
    puts("</table>\n");
    puts("<BR CLEAR=ALL>\n");
    fclose(statfile);
  }
  puts("</BODY></HTML>\n");
  return(0);
}
