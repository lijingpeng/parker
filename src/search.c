/************************************************/
/*  search.c cgi version 4.0			*/
/*  this file is written for search             */
/*                          			*/
/*  improved from Noisy Parker                	*/
/*  author steff	2001/5/21               */
/********************************************
 * Log:
 * 2004/07/04	Hai Xie    - added some info link
 * 2006/02/22	Sunry Chen - fixed bug in visiting HREF URL
*/
#define CGI_VERSION   "2.4"
#include "parker.h"
#include "language.h"
#include "minilzo.h"
#include <ctype.h>

#define ANONYMOUS "anonymous"
#define ANONYPASS "my@my.com"

/* compress functions' define */
int lzo_gets(char *);
FILE *lzo_fopen(const char *);
void lzo_fclose(void);
int lzoInit(void);
void lzo_close(void);

//char CurrentHost[MAX],CurrentPath[MAX];
char CurrentHost[MAX],CurrentHost_t[MAX],CurrentPath[MAX];
struct dirent **namelist;
struct ftpsite site;
int hostNum,collection;
char *str[MAXSTR],*ext[MAXSTR],*not[MAXSTR],*String,*Ext,*Not;
struct CgiArg ArgBack,Arg;
int SearchType=0,iscase=0;
int ff_eof=0;

/* compare 2 str,find the first different, return a relative integer */
int strdiff(const char *a,const char *b)
{ int i,la,lb,dif=0;
  la=strlen(a);
  lb=strlen(b);
  for(i=0;i<la && i<lb; i++){
    if(*(a+i)<*(b+i)){
      dif=la-i;
      break;
    }else if(*(a+i)>*(b+i)){
      dif=i-la;
      break;
    }else
      dif=0;
  }
  return(dif);
}

/* sort the site, make the site which is near the client as the first output */
/* by comparing IP */
int ftpsort(const struct dirent **a, const struct dirent **b)
{
   int d_a_c,d_b_c,d;
   char ClientIP[20];
   strcpy(ClientIP,getenv("REMOTE_ADDR"));
   if(!strncmp(ClientIP,"219.242",7)) strcpy(ClientIP,"219.242.112.0");
   d_a_c=strdiff(ClientIP,a[0]->d_name);
   d_b_c=strdiff(ClientIP,b[0]->d_name);
  d=abs(d_a_c)-abs(d_b_c);
  return(d);
}

/* only the filename is begin with digital is select */
/*                        AND                        */
/* only the filename is begin with letter  is select */
int ftpselect(const struct dirent *input)
{  
  struct stat filestat;
  time_t timenow;
  int days;
  char genbuf[MAX];

//  if(!isdigit(*(input->d_name)))
  if(!isalnum(*(input->d_name)))
  	return(0);
  sprintf(genbuf,"%s/%s/%s",PARKER_HOME,LZOFILEDATADIR,input->d_name);
  if(stat(genbuf,&filestat)!=0)
	return(0);
  time(&timenow);
  days=(timenow - filestat.st_mtime)/86400;
  if(days > VALIDDAYS)
     return(0);
  else 
     return(1);
}

/* prepare for seeking */
int InitSeek()
{ char CollectPath[MAX],HostName[MAX];
  char genbuf[MAX];
  int i,p=0,notl=0,extl=0,strl=0;
  extern int ftpsort();
  extern int ftpselect();

#ifdef DEBUG
  showline=0;
#endif 	
  String=Arg.String;
  Not=Arg.Not;
  Ext=Arg.Ext;
  if(!strcasecmp(Arg.SearchType,SWILDCARDS)){
    SearchType=1;}/* same file name */
  else
    SearchType=0; /* substring */
  if(!strcasecmp(Arg.Case,SCASE)){ 
    iscase=1;/* case sensetive */
  }
  else
    iscase=0;

  str[0]=String; /* devide search key string */
  strl=strlen(String);
  for(i=0;!iscase && i<strl;i++){
        if(*(String+i)>='A' && *(String+i)<='Z') *(String+i)=*(String+i)+32;
  }
  if(strlen(String)>1 && (strchr(String,'*') || strchr(String,' '))){
        while(*String=='*' || *String==' ') String++; /* pass all the ' ' and '*' at begin */
        p=0;
	for(i=0;i<strl;i++){  /* seek string and cut them */
	  if(*(String+i)=='*' || *(String+i)==' '){
	    *(String+i)='\0';
	    if(*(String+i+1)!='\0'){ /* not reach the end */
	      str[++p]=String+i+1;   /* next str */
	      while(*str[p]=='*' || *str[p]==' '){
		str[p]++; /* pass all the ' ' and '*' at begin */
		i++;}
	    }else break; /* find all, finished */
	  }
	}
  }
  str[p+1]=NULL;
  /* devide Not include string */
  if(Not && strlen(Not)>1){
    not[0]=Not; 
    if((strchr(Not,',') || strchr(Not,' ') || strchr(Not,'+'))){
        while(*Not==',' || *Not==' ' || *Not=='+') Not++; /* pass all the ' ' and '*' at begin */
        p=0;
        notl=strlen(Not);
	for(i=0;i<notl;i++){  /* seek Not and cut them */
	  if(*(Not+i)==',' || *(Not+i)==' ' || *(Not+i)=='+' ){
	    *(Not+i)='\0';
	    if(*(Not+i+1)!='\0'){ /* not reach the end */
	      not[++p]=Not+i+1;   /* next not */
	      while(*not[p]==',' || *not[p]==' ' || *not[p]=='+'){
		not[p]++; /* pass all the ' ' and '*' at begin */
		i++;}
	    }else break; /* find all, finished */
	  }
	}
    }
    not[p+1]=NULL;
  }else
    not[0]=NULL;
  /* devide file.extention */
  if(strlen(Ext)>0){
    ext[0]=Ext; 
    if( (strchr(Ext,'+') || strchr(Ext,' ')|| strchr(Ext,','))){
        while(*Ext=='+' || *Ext==' ' || *Ext==',') Ext++; /* pass all the ' ' and '*' at begin */
        p=0;
	extl=strlen(Ext);
	for(i=0;i<extl;i++){  /* seek Ext and cut them */
	  if(*(Ext+i)=='+' || *(Ext+i)==' ' || *(Ext+i)==','){
	    *(Ext+i)='\0';
	    if(*(Ext+i+1)!='\0'){ /* not reach the end */
	      ext[++p]=Ext+i+1;   /* next str */
	      while(*ext[p]=='+' || *ext[p]==' ' || *ext[p]==','){
		ext[p]++; /* pass all the ' ' and '*' at begin */
		i++;}
	    }else break; /* find all, finished */
	  }
	}
    }
    ext[p+1]=NULL;
  }else
    	ext[0]=NULL;
	if(!lzoInit()){
		printf("lzo init err");
		return(3);
	}
  sprintf(CollectPath,"%s/%s",PARKER_HOME,LZOFILEDATADIR);
  hostNum = scandir(CollectPath, &namelist, &ftpselect, &ftpsort); /* read $parker/FtpData dir and sort it.*/
  if(hostNum < Arg.StartPoint.StartHost){
#ifdef DEBUG
    printf("Read files in Dir: %s fails\n",CollectPath); 
#endif
    return(1); /* open dir fail */
  }
  /* seek the Start point */
  strcpy(CurrentHost,namelist[Arg.StartPoint.StartHost-1]->d_name);
  sprintf(HostName,"%s/%s/%s",PARKER_HOME,LZOFILEDATADIR,CurrentHost);
  if(!lzo_fopen(HostName)){
    #ifdef DEBUG
        printf("Opening file %s error!<br>\n",HostName);
        fflush(stdout);
    #endif
	ff_eof=1;
    return(2); /* open ftp data fails */
  }
	ff_eof=0;
  strcpy(CurrentPath,"/");
  for(i=1;i<Arg.StartPoint.StartLine;i++){
    if(lzo_gets(genbuf)<0){ /*seek data to start line */
	ff_eof=1;
	break;
    }
    if(*genbuf=='/') strcpy(CurrentPath,genbuf);
  }
  return(0);
}

void CloseSeek()
{ 
	lzo_close();
}

void Chop(char *s)
{
  char *ptr;

  if((ptr=strchr(s,CR))!=NULL) *ptr='\0';
  if((ptr=strchr(s,LF))!=NULL) *ptr='\0';
}


/* quick find string2 in string1 */
int qstrstr(const char *str1, const char *str2)
{ int i,j;
  for(i=0,j=0;  j<MAX && i<MAX;i++,j++){
    if(*(str2+j)=='\0') return(1);
    if(*(str1+i)=='\0') return(0);
    if(*(str1+i)!=*(str2+j)) j=0;
  }
  return(0);
}

/* same file name find */
int Wildcards(char *genbuf)
{ int flag;
  if(iscase)
    flag=!strcmp(genbuf,String);
  else
    flag=!strcasecmp(genbuf,String);
  return(flag);
}

/* substring find                 */
/* can use more then one key word */
/* exclude, filetype with define  */
/* the filename extention         */
int Substring(char *genbuf)
{
  int genl,strl,extl;
  int flag,p,i,j;
  char s1;

  #ifdef DEBUG
    /* printf("look \"%s\" in \"%s\".",String,genbuf); */
    showline++;
  #endif

  if(not[0])
  for(p=0; not[p] && p<MAXSTR ;p++){ /* check Not include */
    if(qstrstr(genbuf,not[p])){
      return(0);
    }
  }
  if(ext[0])
  { flag=0;
    genl=strlen(genbuf);
    strl=strlen(String);
    for(p=0;ext[p] && p<MAXSTR;p++) /* check extention name */
    { extl=strlen(ext[p]);
      if(!strcasecmp(genbuf+(genl-extl),ext[p])) {
        flag=1;
        break;
      }
    }
    if(flag==0)
      return(0);
  }
  for(p=0;str[p] && p<MAXSTR;p++){ /* check string */
    for(i=0,j=0;*(str[p]+j)!='\0';i++){
      if(*(genbuf+i)=='\0') return(0);
      if(i>MAX-2) return(0);
      s1=*(genbuf+i); 
      if(!iscase){
        if(s1>='A' && s1<='Z') s1=s1+32;
      }
      if(s1!= *(str[p]+j)) 
        j=0;
      else
        j++;
    }
  }
  return(1);
}

/* call by Search */
/* seek the next matched file */
/* use wildcard or substring */
int Query( char *filename, long *filesize, char *filetime)
{
  char genbuf[MAX],HostName[MAX];
  int  flag=0,j;
  while(1){
    if(ff_eof){
      	lzo_fclose();
      	Arg.StartPoint.StartHost++;
      	if(Arg.StartPoint.StartHost>(hostNum)){
        return(0);         /* if end of host list , return fauls */
      }else{
         strcpy(CurrentHost,namelist[Arg.StartPoint.StartHost-1]->d_name);
         sprintf(HostName,"%s/%s/%s",PARKER_HOME,LZOFILEDATADIR,CurrentHost);
         if(!lzo_fopen(HostName)){  /* open host data file */
           continue;
         }
	ff_eof=0;
         Arg.StartPoint.StartLine=1;
         strcpy(CurrentPath,"/");
      }
    } else{                 /* read data file, find str, until end file */
      if(lzo_gets(genbuf)<=0){
	ff_eof=1;
	continue;
      }
      Arg.StartPoint.StartLine++;
      if(*genbuf=='/'){      /* it's a path */
        strcpy(CurrentPath,genbuf);
      }
      else{                 /* it's a file or directory */
        for(j=0;*(genbuf+j)!=' ' && *(genbuf+j)!='\0' && j<MAX ;j++)
          *(filename+j)=*(genbuf+j);
        *(filename+j)='\0';
        if(SearchType==1){
          flag=Wildcards(filename);
        }else
          flag=Substring(filename);
        if(flag){
          sscanf(genbuf,"%s %lu %s",filename,filesize,filetime);
          break;
        }else
          continue;
      }
    }
  }/*  then loop */
  return(1);
}

void EnConv(char *s)/* encode ascii to URL string */
{
  for(;*s;s++) if(isalpha(*s) || isdigit(*s)) printf("%c",*s);
               else printf("%%%02x",*s);
}


void Conv(char *s)   /* convert ascii generated by IE,Netscape */
{
  char num[3];
  int n;

  if(!s) return;
  num[2]='\0';
  for(;*s;s++)
  {
    if(*s=='+') *s=' ';
    else if(*s=='%') {
      num[0]=*(s+1);
      num[1]=*(s+2);
      sscanf(num,"%x",&n);
      *s=n;
      strcpy(s+1,s+3);
    }
  }
}
void nameConv(char *s)   /* convert ascii generated by IE,Netscape */
{
  char num[3];
  int n;

  if(!s) return;
  num[2]='\0';
  for(;*s;s++)
  {
    if(*s=='%') {
      num[0]=*(s+1);
      num[1]=*(s+2);
      sscanf(num,"%x",&n);
      *s=n;
      strcpy(s+1,s+3);
    }
  }
}

void PutHeaders(void)
{
  puts("Content-type:text/html\n\n");
  printf("<html><head><TITLE>%s</TITLE></head>\n",UserLanguage->HSearchResult);
  printf("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" >\n");
  puts("<BODY BGCOLOR=#ffffff>\n");
//  puts("<IMG SRC=\"/images/parker.gif\" ALIGN=RIGHT>\n");
  printf("<div><dir><H2><a href=\"/\">%s</a></H2>",UserLanguage->HHtmlTitle);
  puts("<style>");
  puts("a {  color: rgb(0,0,150) ; Text-decoration:none  ; font-weight : none}");
  puts("a:hover {color: rgb(250,40,40);text-decoration:underline; font-weight : none}");
  puts("</style>");
  fflush(stdout);
}
/* call by main */
/* call Query the find matched file */
int Search(void)
{
  unsigned long shootedfiles=0;
  char filename[MAX],filetime[MAX];
  unsigned long filesize=0;
  int  p,error=0,flag=0;
  char lastfile[MAX],lasthost[MAX],lastpath[MAX];
  char dnsname[MAX],timestr[MAX],genbuf[MAX];
  char *ptr;

  if((error=InitSeek())){
    printf("Internal error: %d  ",error);
    return(error);
  }
  if(!strcmp(Arg.SearchType,SSUBSTRING)){
    printf("%s <font color='#990000'>%s</font> %s",UserLanguage->HUse,UserLanguage->HKeyword,UserLanguage->HToSearch);
    for(p=0;  str[p]; p++){
      printf(" %s<font color='#990000'>%s</font>%s ",UserLanguage->HLeftQuation,str[p],UserLanguage->HRightQuation);
    }
    printf("%s<br>\n",UserLanguage->HSubstring);
    printf("%s%s",(strcmp(Arg.Case,SCASE)==0)? UserLanguage->HCase : UserLanguage->HIsNotCase,UserLanguage->HCaption);
    if(not[0]) {
      printf(",%s",UserLanguage->HExclude);
      for(p=0;  not[p]; p++)
        printf(" %s<font color='#990000'>%s</font>%s ",UserLanguage->HLeftQuation,not[p],UserLanguage->HRightQuation);
    }
    if(ext[0]){
      printf(",%s",UserLanguage->HExtNameIs);
      for(p=0;  ext[p]; p++){
        printf(" %s<font color='#990000'>%s</font>%s ",UserLanguage->HLeftQuation,ext[p],UserLanguage->HRightQuation);
      }
    }
  }else
    printf("<s3>%s %s<font color='#990000'>%s</font>%s %s", UserLanguage->HSearchFileNamed1, UserLanguage->HLeftQuation,String,UserLanguage->HRightQuation,UserLanguage->HSearchFileNamed2);
  printf("</font>. %s:<s3></div>\n",UserLanguage->HThisSearchResult);
  printf("<P><font size=2>\n");
  puts("<dir>\n");
  fflush(stdout);
#ifdef DEBUG
    while(showline<50 && shootedfiles<Arg.Hits && (error=Query(filename, &filesize, filetime)))
#else
    while(shootedfiles<Arg.Hits && (flag=Query( filename, &filesize, filetime)))
#endif
    {
      shootedfiles++;
      error=0;
      if(strcmp(CurrentHost,lasthost)) { /* Another ftp site */
        strcpy(lasthost,CurrentHost);
        error=1;
        if (cmpfiletime(CurrentHost))
          {}//puts("<font size=4><IMG SRC=\"/images/host.gif\" WIDTH=32 HEIGHT=32>");
        else
          {;}//printf("<font size=4><IMG SRC=\"/images/badhost.gif\" WIDTH=32 HEIGHT=32 alt='%s'>",UserLanguage->HLive);
//        puts("<font size=4><IMG SRC=\"/images/host.gif\" WIDTH=32 HEIGHT=32>");
//        if(getdnsname(CurrentHost, dnsname))
// added by Sunry Chen
        strcpy(CurrentHost_t,CurrentHost);
        scanline(CurrentHost_t);

	if(getdnsname(site.ftp_name, dnsname))
	  printf("<A HREF=\"ftp://%s/\"><b>%s</b></A></font>",CurrentHost,dnsname);
        else 
          printf("<A HREF=\"ftp://%s/\"><b>%s</b></A></font> ",CurrentHost,CurrentHost);
        if(getfiletime(CurrentHost, timestr,UserLanguage))
          printf(" %s:%s\n",UserLanguage->HUpdata,timestr);
//	printf("&nbsp;&nbsp<A HREF='siteinfo?%s'><i>%s</i></A>",CurrentHost,UserLanguage->HInfo);
        puts("<br>");
        fflush(stdout);
      }
      if(error || strcmp(CurrentPath,lastpath)) { /* Another Directory */
        strcpy(lastpath,CurrentPath);
	strcpy(genbuf,CurrentPath);
        nameConv(genbuf);
        puts("&nbsp;&nbsp;<IMG SRC=\"/images/dir.gif\" WIDTH=16 HEIGHT=12>");
        printf(" <A HREF=\"ftp://%s%s\">%s</A><BR>\n",CurrentHost,lastpath,genbuf);
        fflush(stdout);
      }
      strcpy(lastfile,filename);
      nameConv(filename);                              /* convert %20 -> ' ' */
      if(*(ptr=filename+strlen(filename)-1)=='/'){ /* is a directory */
        puts("&nbsp;&nbsp;<IMG SRC=\"/images/diritem.gif\" WIDTH=42 HEIGHT=12>");
        printf(" <A HREF=\"ftp://%s%s%s\">%s</A>\n",CurrentHost,lastpath,lastfile,filename);
        printf(" <EM>%s</EM><br>",filetime);
        fflush(stdout);
      }
      else{ 
          puts("&nbsp;&nbsp;<IMG SRC=\"/images/file.gif\" WIDTH=40 HEIGHT=12>");
          printf(" <A HREF=\"ftp://%s%s%s\">%s</A>\n",CurrentHost,lastpath,lastfile,filename);
          printf(" <EM>%lu</EM>%s",filesize,UserLanguage->HBytes);
          printf(", <EM>%s</EM><br>",filetime);
          fflush(stdout);
        }
      fflush(stdout);
      wait(NULL);
    }
    puts("</font></dir><BR>\n");
    if(!shootedfiles) 
      printf("%s<BR>\n",UserLanguage->HSorryNothingFound);
    else {
      printf("<BR><B>%s  %lu  %s\n", UserLanguage->HHaveList,shootedfiles,UserLanguage->HHFiles);
	fflush(stdout);
      if(flag) { /* Can find more */
//        printf(", %s<A HREF=\"/cgi-bin/cgftpunion/search?",UserLanguage->HPressHere);
        printf(",  %s<A HREF=\"/cgi-bin/ftp/search?",UserLanguage->HPressHere);
	printf("String=%s",ArgBack.String);
        printf("&Not="); EnConv(ArgBack.Not);
        printf("&Ext="); EnConv(ArgBack.Ext);
        printf("&Hits=%d",ArgBack.Hits);
        printf("&SearchType="); EnConv(ArgBack.SearchType);
        printf("&Case=");EnConv(ArgBack.Case);
        printf("&StartPoint=%d,%d",Arg.StartPoint.StartHost,Arg.StartPoint.StartLine);
        printf("\">%s</A>\n",UserLanguage->HGetMore);
        fflush(stdout);
      }
    }
  CloseSeek();
  return(flag);
}

void PutEnd()
{
//printf("<hr size=3><FORM ACTION='/cgi-bin/cgftpunion/search' ID='s'><h2><font color=#9C0000>%s</font></h2>\n",UserLanguage->HNewSearch);
printf("<hr size=3><FORM ACTION='/cgi-bin/ftp/search' ID='s'><h2 align='center'><font color=#9C0000>%s</font></h2>\n",UserLanguage->HNewSearch);
printf("<table align='center'><tr><td><ALIGN='center'>\n");
printf("	    <SELECT NAME='SearchType' >\n");
if(SearchType){
printf("		<OPTION VALUE='wildcards'>%s\n",UserLanguage->HFullFileName);
printf("		<OPTION VALUE='substring'>%s\n",UserLanguage->HFileNameInclude);
}else{
printf("		<OPTION VALUE='substring'>%s\n",UserLanguage->HFileNameInclude);
printf("		<OPTION VALUE='wildcards'>%s\n",UserLanguage->HFullFileName);
}
printf("	    </SELECT>");
printf("		<INPUT TYPE='text' NAME='String' SIZE='25' value=");
printf("'%s'",ArgBack.String);puts(">");
//printf("		<INPUT TYPE='submit' VALUE='%s  '>\n",UserLanguage->HGoSearch);
printf("	%s<INPUT TYPE='text' NAME='Not' SIZE='10' width='200' value=",UserLanguage->HExclusion);
printf("'%s'",ArgBack.Not);puts(">");
printf(" 	%s<INPUT TYPE='text' NAME='Ext' SIZE='10' width='200' value=",UserLanguage->HExtName);
printf("'%s'",ArgBack.Ext);puts(">");
printf("	%s\n",UserLanguage->HSearchType);
printf("	    <SELECT NAME='Case'>\n");
if(iscase){
printf("		<OPTION VALUE='case'>%s\n",UserLanguage->HIsCaseCaption);
printf("		<OPTION VALUE='incase'>%s\n",UserLanguage->HIsNotCaseCaption);
}else{
printf("		<OPTION VALUE='incase'>%s\n",UserLanguage->HIsNotCaseCaption);
printf("		<OPTION VALUE='case'>%s\n",UserLanguage->HIsCaseCaption);
}
printf("	    </SELECT>\n");
printf("	   %s<SELECT NAME='Hits'>\n",UserLanguage->HResultsOneScreen);
printf("		<OPTION VALUE='50'>50\n");
printf("		<OPTION VALUE='100'>100\n");
printf("		<OPTION VALUE='500'>500\n");
printf("	    </SELECT>\n");
printf("		<INPUT TYPE='submit' color=red VALUE='New Search %s  '>\n",UserLanguage->HGoSearch);
printf("      </td></tr>\n");
printf("  <tr><td  COLSPAN='5'><dir><dir><BR>\n");
printf("</table></form>\n");
printf("<div align=center>");
printf("<font color=gray >Copyright &copy HIT 2013 %s\n",UserLanguage->HProductby);
printf("<A HREF=\"/\" TARGET=_top>%s</A>.",UserLanguage->HHostName);
printf("</div>");
printf("</BODY></HTML>");
fflush(stdout);
}

// Convert HEX to Decade
int HEX_TO_DECADE(char* s)
{
	char *digits="0123456789ABCDEF";
	if (islower (s[0]))
		s[0]=toupper(s[0]);
	if (islower (s[1]))
        s[1]=toupper(s[1]);
    
	return 16*(strchr(digits,s[0])-strchr(digits,'0'))+(strchr(digits,s[1])-strchr(digits,'0'));
}

// Convert charset before search
void ConvertCharset(char* src, char* dst)
{
	char tmp[3];
	int len = strlen(src);
	int i,j;
	for( i=0, j=0; i < len; j++)
	{
		if(src[i] == '%')
		{
			tmp[ 0 ] = src[ i+1 ];
			tmp[ 1 ] = src[ i+2 ];
			tmp[ 2 ] = '\0';
			dst[ j ] = HEX_TO_DECADE( tmp );
			i += 3;
		}
		else if( src[i] == '+' )
		{
			dst[ j ] = src[ i ];
			++i;
		}
		else
		{
			dst[ j ] = src [ i ];
			++i;
		}
	}
	dst[j] = '\0';	

}

int main(void)
{
  char *query,*pptr,*pptr2;
  char *User_Language;
  char genbuf[MAX];
  int error=0;

  alarm(CGIALARM);
  /*User_Language=getenv(ACCEPT_LANGUAGE);
  if(User_Language && *User_Language && strstr(User_Language,"zh-cn"))
  {
  	UserLanguage=&ChineseDef;
  }
  else
  	UserLanguage=&EnglishDef;*/
  UserLanguage=&EnglishDef;
  query=getenv(QUERYSTRING);
//query="SearchType=substring&String=春节&Submit=&Not=&Ext=&Case=incase&Hits=100";
  PutHeaders();
  //printf("%s\n\n", query);

  *Arg.String='\0';
  *Arg.Ext='\0';
  *Arg.Not='\0';
  Arg.Hits=50;
  strcpy(Arg.SearchType,SSUBSTRING);
  strcpy(Arg.Case,SINCASE);
/*  strcpy(Arg.Collection,"All"); */
  Arg.StartPoint.StartHost=1;
  Arg.StartPoint.StartLine=1;
  
  
  if(query && *query )
  {
    if(!strchr(query,'&'))
    { 
	if(!strncmp(query,"String=",7)) query+=7;
        strncpy(Arg.String,query,MAX);
    }
    else
    {
      while(query && *query)
      {
        if(!strncmp(query,"String=",7)) 
	{
	  //String
          query+=7;
          strncpy(Arg.String,query,MAX);
          if((pptr=strchr(Arg.String,'&'))!=NULL) *pptr='\0'; /* set the NULL at end of string */
          //printf("<p>---------------------------------------</p>");
          //iiprintf("after strncmp, query=%s<br>\n", query);
	  //printf("Arg.String=%s<br>\n", Arg.String);
        }
	else if(!strncmp(query,"Not=",4))
	{
          query+=4;
          strncpy(Arg.Not,query,MAX);
          if((pptr=strchr(Arg.Not,'&'))!=NULL) *pptr='\0';
        }
	else if(!strncmp(query,"Ext=",4))
	{
          query+=4;
          strncpy(Arg.Ext,query,MAX);
          if((pptr=strchr(Arg.Ext,'&'))!=NULL) *pptr='\0';
        }
	else if(!strncmp(query,"Hits=",5))
	{
          query+=5;
          strncpy(genbuf,query,MAX);
          if((pptr=strchr(genbuf,'&'))!=NULL) *pptr='\0';
          sscanf(genbuf,"%d",&Arg.Hits);
          if(Arg.Hits < 1)
            Arg.Hits=100;
          else if(Arg.Hits > 1000)
            Arg.Hits=1000;
        }
	else if(!strncmp(query,"Case=",5)) 
	{
          query+=5;
          if( !strncmp(query, SCASE,strlen(SCASE)))
            strcpy(Arg.Case,SCASE);
          else 
            strcpy(Arg.Case,SINCASE);
        }
	else if(!strncmp(query,"SearchType=",11)) 
	{
          query+=11;
          if( !strncmp(query, SWILDCARDS,strlen(SWILDCARDS)))
            strcpy(Arg.SearchType,SWILDCARDS);
          else 
            strcpy(Arg.SearchType,SSUBSTRING);
/*        } else if(!strncmp(query,"Collection=",11)) {
          query+=11;
          strcpy(Arg.Collection,query);
          if((pptr=strchr(Arg.Collection,'&'))!=NULL) {*(pptr)='\0';}*/
        } 
	else if(!strncmp(query,"StartPoint=",11)) 
	{
          query+=11;
          strcpy(genbuf,query);
          if((pptr=strchr(genbuf,','))!=NULL){
            *pptr='\0';
            sscanf(genbuf,"%d",&Arg.StartPoint.StartHost);
	    pptr++;
          }
          if((pptr2=strchr(pptr,'&'))!=NULL) *pptr2='\0';
          sscanf(pptr,"%d",&Arg.StartPoint.StartLine);
        }
        query=strchr(query,'&');
        if(query) query++;
      }
    }
  }
// Convert Charset
 /* printf("<p> before convert </p>");
  printf("String:%s<br>\n",Arg.String);
  printf("NOT:%s<br>\n",Arg.Not);
  printf("Ext:%d<br>\n",Arg.Ext);
  printf("Hits:%d<br>\n",Arg.Hits);
  printf("StartPoint:%d,%d<br>\n",Arg.StartPoint.StartHost,Arg.StartPoint.StartLine);
*/
  char tmpCov[ MAX ];
  ConvertCharset(Arg.String, tmpCov);
  strncpy(Arg.String, tmpCov, strlen(tmpCov) + 1);
  //Conv(Arg.String);
  ConvertCharset(Arg.Not, tmpCov);
  strncpy(Arg.Not, tmpCov, strlen(tmpCov) + 1);
  
  ConvertCharset(Arg.Ext, tmpCov);
  strncpy(Arg.Ext, tmpCov, strlen(tmpCov) + 1);

  //Conv(Arg.Not);
  //Conv(Arg.Ext);
  memcpy(&ArgBack,&Arg,sizeof(Arg));

  /* Convert from the `%' notation 
//  #ifdef DEBUG
    	printf("<p> after convert </p>");
    	printf("String:%s<br>\n",Arg.String);
        printf("NOT:%s<br>\n",Arg.Not);
  	printf("Ext:%s<br>\n",Arg.Ext);
	printf("Hits:%d<br>\n",Arg.Hits);
	printf("StartPoint:%d,%d<br>\n",Arg.StartPoint.StartHost,Arg.StartPoint.StartLine);
  //#endif*/
  if(strlen(Arg.String)>=MINLENGTHOFSEARCH)
  {
    if(Arg.StartPoint.StartHost*Arg.StartPoint.StartLine==1)
    { /* log only once */
      sprintf(genbuf,"Seach:'%s' ",Arg.String);
      if(strcmp(Arg.SearchType,SSUBSTRING)) strcat(genbuf,SWILDCARDS);
      if(*Arg.Not){ strcat(genbuf,",Not:"); strcat(genbuf,Arg.Not);}
      if(*Arg.Ext){ strcat(genbuf,",Ext:"); strcat(genbuf,Arg.Ext);}
      Log(SEARCHLOGFILE,genbuf);
      onlyLog(SEARCHKEYLOGFILE,Arg.String);
    }
    error=Search();
  }
  PutEnd();
  return(error);
}


