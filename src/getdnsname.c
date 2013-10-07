/************************************************/
/*  getdnsname.c version 4.0    		*/
/*  this function is write for collect.pl	*/
/*  command program 				*/
/*  input IP and return DNS name 		*/
/*  author steff	2001/5/4                */
/************************************************/
#include "parker.h"
extern int h_errno;

    void usage(char *arg)
    {
    printf("Get DNS name from IP.\n");
    printf("Usage: %s <IP>\n", arg);
    exit(1);
    }

int main(int argc, char *argv[])
{
  char dnsname[MAX];
  unsigned char addr[4];

  if(argc < 2){
    usage(argv[0]);
    exit(1);
    }
  if(getftpname(argv[1],dnsname,addr)==3)
    printf("%s\n",dnsname);
   
  else
    printf("\n");
  return(0);
}

