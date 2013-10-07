/************************************************/
/*  getftp.c function version 4.0		*/
/*  this file is written for get data property  */
/*  call by search,statist      		*/
/*  improved from Noisy Parker                	*/
/*  author steff	2001/5/4                */
/************************************************/
#include "parker.h"
extern int h_errno;
struct ftpsite site;
char machine[MAX];

int getfiletime(char *host, char *timestr,struct LanguageDef *UserLanguage)/* return last collect action time */
{ char FileName[MAX];
  struct stat filestat;
  struct tm *filetm;
  sprintf(FileName,"%s/%s/%s",PARKER_HOME,LZOFILEDATADIR,host);
  if(!stat(FileName,&filestat)){
    filetm=localtime(&filestat.st_mtime);
    sprintf(timestr,"%d%s%d%s%d%s",filetm->tm_year+1900,UserLanguage->HYear,filetm->tm_mon+1,UserLanguage->HMonth,filetm->tm_mday,UserLanguage->HDay);
    return(1);
  }else
    return(0);
}

int cmpfiletime(char *host)/* return last collect action time */
{ char FileName[MAX];
  struct stat filestat;
  time_t now;
  now = time(0);
  sprintf(FileName,"%s/%s/%s",PARKER_HOME,LZOFILEDATADIR,host);
  if(!stat(FileName,&filestat)){
    if ((now-filestat.st_mtime) < 28800)
      return(1);
    else
      return(0);
  }
  return (-1);
}


/* use $parker/HostData/ip.host to get DNS name */
int getdnsname(char *ip, char *dnsname)
{ char genbuf[MAX];
  FILE *fd;
  sprintf(genbuf,"%s/%s/%s.%s",PARKER_HOME,HOSTDATADIR,ip,HOSTINFOEXT);
  if((fd=fopen(genbuf,"r"))){
    fscanf(fd,"%s",dnsname);
    if(strlen(dnsname)>1)
      return(1);
  }
  strcpy(dnsname,ip);
  return(0);
}

/* this function may cause fail at some ip. */
/* use online socket call to get DNS name */
int getftpname(char *input, char *dnsname, unsigned char *addr)
{ /* unsigned char addr[4]; */
  int i;
  struct hostent *hostP;
  
  if(!input) return(-2); /* wrong input */
  strcpy(dnsname,input);
  sethostent(0);
  if(!(hostP=gethostbyname(input)))
  {  endhostent();
/*    printf("gethostbyname error!\n"); */
    return(-5);} /* can do nothing */
  for(i=0;i<4;i++) 
    addr[i]=hostP->h_addr[i];
  if(getclass(addr)<0){
    return(-1);}
/*  printf("h_error (1) error=%d.\n",h_errno); */
  if(h_errno>1){
/*    printf("%d,%d,%d,%d,%d\n",HOST_NOT_FOUND,NO_ADDRESS,NO_DATA,NO_RECOVERY,TRY_AGAIN);*/
    return(0);    /* can only name -> addr */
    }
  if(!(hostP=gethostbyaddr(addr,4, AF_INET)) )
  {  endhostent();
/*    printf("gethostbyaddr error!\n"); */
    return(1);}  /* can't addr -> name */
  endhostent();
/*  printf("herror(2) error:%d\n",h_errno); */
  if(h_errno>1){
    }
  if(!hostP->h_name){
/*    printf("hostP->name is NULL\n"); */
    return(2);} /* null host name */
  strcpy(dnsname,hostP->h_name);
  return(3); /* ok */
}

/* return 1 if the 'addr' is in 'collect' */
/* 'collect' is defined in $parker/HostData/collect.mask */
int isinrange(char *collect, unsigned char *addr)
{
  char filename[MAX];
  char genbuf[MAX];
  char *pptr,*pptr2;
  unsigned int host[4],mask[4],tmp[4];
  unsigned int tempaddr[4];
  int i,flag;
  FILE *fd;
  flag=0;
  sprintf(filename,"%s/%s/%s.%s",PARKER_HOME,HOSTDATADIR,collect,MASKFILEEXT);
  if(!(fd=fopen(filename,"r"))){
    return(0);}
  while(fd && !feof(fd)){
    fgets(genbuf,MAX,fd);
    pptr2=genbuf;
    if((pptr=strchr(genbuf,'/'))) *pptr='.'; else continue;
    for(i=0;i<4;i++){
      if((pptr=strchr(pptr2,'.'))) *pptr='\0'; else continue;
      sscanf(pptr2,"%u",&tempaddr[i]);
      host[i]=tempaddr[i];
      pptr2=pptr+1;
    }
    for(i=0;i<4;i++){
      if((pptr=strchr(pptr2,'.'))) *pptr='\0'; 
      sscanf(pptr2,"%u",&tempaddr[i]);
      mask[i]=tempaddr[i];
      pptr2=pptr+1;
    }
    flag=1;
    for(i=0;i<4;i++){
      tmp[i]=addr[i]&mask[i];
      if(tmp[i]!=host[i]){
        flag=0;
        break;
      }
    }
    if(flag){
/*      printf("co:%s,net:%d.%d.%d.%d,mask:%d.%d.%d.%d,ip:%d.%d.%d.%d\n",collect,host[0],host[1],host[2],host[3],mask[0],mask[1],mask[2],mask[3],addr[0],addr[1],addr[2],addr[3]);
*/
      break;
    }
  }
  fclose(fd);
  return(flag);         
}

/* convert IP from string to unsigned char format */
int string2addr(char *ipstring, unsigned char *addr)
{ 
  int i;
  char *pptr2,*pptr;
  unsigned int tempaddr[4];

  pptr2=ipstring;
    for(i=0;i<4;i++){
      if((pptr=strchr(pptr2,'.'))) *pptr='\0'; 
      sscanf(pptr2,"%u",&tempaddr[i]);
      addr[i]=tempaddr[i];
      pptr2=pptr+1;
    }
  return(1);
}

/* get which class the addr is           */
/* like in Nanjing University, return 1  */

/* Chinese Education network, return 2   */
/* out of free IP range, return 3        */
/* can only visit in school,  return -1  */
int getclass(unsigned char *addr)
{ 
  if(isinrange("Inn",addr)){
    return(-1);}
  else if(isinrange(LOCOL,addr))
    return(1);
  else if(isinrange(EDUCOL,addr)){
    return(2);}
  else{
/*    printf("ip:%d.%d.%d.%d\n",addr[0],addr[1],addr[2],addr[3]); */
    return(3);
  }
}


void
scanline (char line[])
{
  char *scan, *at, *colon;
  site.ftp_name[0]='\0';
  site.ftp_port[0]='\0';
  site.ftp_user[0]='\0';
  site.ftp_pass[0]='\0';
  if ((scan = strchr (line, CR)) != NULL)
	  *scan = '\0';
  if ((scan = strchr (line, LF)) != NULL)
	  *scan = '\0';
  scan = strstr (line, "ftp://");
  if (!scan)
    scan = line;
  else
    scan = line + 6;
  at = strchr (scan, '@');
  colon = strchr (scan, ':');
  if (!at)
    {
      if (colon)
	{
	  *colon = '\0';
	  colon++;
	  strcpy (site.ftp_port, colon);
	}
      strcpy (site.ftp_name, scan);
      strcpy (machine, site.ftp_name);
    }
  else if (at != NULL)
    {
      *at = '\0';
      at++;
      if (colon && colon < at)
	{
	  *colon = '\0';
	  colon++;
	  strcpy (site.ftp_pass, colon);
	}
      strcpy (site.ftp_user, scan);
      colon = strchr (at, ':');
      if (colon)
	{
	  *colon = '\0';
	  colon++;
	  strcpy (site.ftp_port, colon);
	}
      strcpy (site.ftp_name, at);
      strcpy (machine, site.ftp_name);
    }
  if (site.ftp_port[0] == '\0')
	  strcpy (site.ftp_port, "21");
  if (site.ftp_user[0] == '\0')
	  strcpy (site.ftp_user, "anonymous");
  if (site.ftp_pass[0] == '\0')
	  strcpy (site.ftp_pass, "my@my.com");
}

/* add ftp site to $parker/etc/onworkhost */
int addftp(char *ipstring)
{
  char fileftp[MAX],genbuf[MAX],ftphost[20], dnsname[MAX];
  FILE *fd;
  unsigned char addr[4];
  int ret=0;
  int flag=1; /* flag=0,not included,success */
              /* flag=1,have included, needn't add */
  sprintf(fileftp,"%s",HOSTLIST);
  if((fd=fopen(fileftp,"r"))){
    flag=0;
    while(!feof(fd)){
      fscanf(fd,"%s",genbuf);
      scanline(genbuf);
      ret=getftpname(machine,dnsname,addr);
      if(ret>-2){
	      sprintf(ftphost,"%d.%d.%d.%d",addr[0],addr[1],addr[2],addr[3]);
      }		    
      if(!strncasecmp(ftphost,ipstring,strlen(ipstring))){
        flag=1;
        break;
      }
    }
    fclose(fd);
  } else
  {
    flag=0;
  } 
  if(!flag) {
    if((fd=fopen(fileftp,"a"))){
        fprintf(fd,"%s\n",ipstring);
        fclose(fd);  
    }else
    {  printf("Write file %s fail!\n",fileftp);
      return(2);
    }
  }
  return(flag);
}
