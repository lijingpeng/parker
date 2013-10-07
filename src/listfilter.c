/************************************************/
/*  convertsite.c  version 4.0
    this file is write for convert ftp site file
    data format:
{   comment:ip       somethine
    ip
}   like 
{   Nju Ftp:202.119.32.33
    202.119.46.240
}
    output file is $parker/etc/onworkhost
     author steff     2001/5/4                  
 *
 * changed the data format by xxxss:
 * ftp://ip/
************************************************/
#include "parker.h"
extern int h_errno;


    void usage(char *arg)
    {
    printf("Ftp site list convert.\n");
    printf("Usage: %s <input file name>\n", arg);
    exit(1);
    }

int main(int argc, char *argv[])
{struct hostent *hostP;

  int ipclass,inlib,i;
  char ftphost[MAX],genbuf[MAX], *ptr,comment[MAX],ipstring[MAX];
  unsigned char addr[4];
  FILE *fd;

  if(argc < 2){
    usage(argv[0]);
    exit(1);
    }

  if((fd=fopen(argv[1],"r")))
    while(!feof(fd)){
      fgets(genbuf,MAX,fd);
      if((ptr=strstr(genbuf,"ftp://"))){
//        *ptr='\0';
        ptr += 6;
	strcpy(genbuf,ptr);
	if((ptr=strchr(genbuf,'/')))
		*ptr='\0';
	if((ptr=strchr(genbuf,' ')))
		*ptr='\0';
//no use, will remove in the future
      }else if((ptr=strstr(genbuf,"£º")) ){
        *ptr='\0';
	ptr += 2;
      }
//      strcpy(comment,genbuf);
      sscanf(genbuf,"%s",ftphost);
      if(!strlen(ftphost)) {
        continue;}
      else{
        if((hostP=gethostbyname(ftphost))){
          for(i=0;i<4;i++) 
            addr[i]=hostP->h_addr[i];
          sprintf(ipstring,"%d.%d.%d.%d",addr[0],addr[1],addr[2],addr[3]); 
          ipclass=getclass(addr);
          if(ipclass==-1)
            inlib=1;
          else if(ipclass==2 || ipclass==1)
            inlib=addftp(ipstring);
          else if(ipclass==3)
            printf("out of free IP:%s\n",ipstring);
        }else{
          herror(ftphost);
          puts(ftphost);  
          printf("%d:\n",h_errno);
        }
     }
  }/* while */
  return(0);
}

