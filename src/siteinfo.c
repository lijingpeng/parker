/************************************************/
/*  siteinfo.c cgi version 0.1			*/
/*  this file is written for FTP site info	*/
/* 						*/
/*                  				*/
/*  author xxxss	2004/7/06               */
/************************************************/
#define CGI_VERSION   "0.1"

#include "parker.h"
#include "language.h"
int main(void)
{
  char *query;
  char genbuf[MAX],buf[MAX],timestr[MAX];
//  unsigned long files=0,rms=0,mp3=0,winfiles=0,linuxfiles=0;
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
  printf("<html><head><TITLE>%s</TITLE></head>\n",UserLanguage->HInfo);
  puts("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=gb2312\">\n");
  puts("<BODY BGCOLOR=#ffccaa  align=left>\n");
  printf("<div align=center><H1>%s</H1></div>",UserLanguage->HInfo);

  if(query && *query && *query!='.' )
    sprintf(genbuf,"%s/%s/%s",PARKER_HOME,SITEINFODIR,query);
  else
    sprintf(genbuf,"%s/%s/%s",PARKER_HOME,SITEINFODIR,"all");
  if(!(statfile=fopen(genbuf,"r"))){
#ifdef DEBUG
    printf("repuest file: %s\n",genbuf); 
#endif
    printf("No Data for this request.\n");
  }else{    
    puts("<table border=3 cellspacing=4 align=center >\n");
    while( fgets(buf,MAX,statfile)!=NULL ){	    
      printf("<tr align='center' ><td nowrap>%s</td></tr>",buf);
    }
    getfiletime(query, timestr,UserLanguage);
    printf("<tr align='center' ><td nowrap>更新时间: %s</td></tr>",timestr);
    puts("</table>\n");
    puts("<BR CLEAR=ALL>\n");
    fclose(statfile);
  }
  puts("</BODY></HTML>\n");
  return(0);
}
