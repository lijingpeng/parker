/************************************************/
/*  convertmask.c  version 4.0
    this file is write for convert free IP list 
    download from CERNIC
    format:
    
   like 
{   202.119.32.0  0.0.31.255 255.255.224.0
}
    output file is $parker/etc/onworkhost
     author steff     2001/5/4                  */
/************************************************/
#include "parker.h"
extern int h_errno;


    void usage(char *arg)
    {
    printf("Free IP list convert.\n");
    printf("Usage: %s <Free IP list file from NIC> [out put file]\n", arg);
//    printf("Default output file is $parker/etc/onworkhost.\n");
    printf("Default output file is $parker/HostData/Edu.mask\n");
    printf("File format ");
    printf("Like this:\n");
    printf("network-number              host-bits      net mask\n");
    printf(" 24.244.192.0              0.0.15.255    255.255.240.0  ");
    printf("\n");
    exit(1);
    }

int main(int argc, char *argv[])
{ 
  int i;
  FILE *fd, *outfd;
  char *pptr1,*pptr2,*ptr1,*ptr2;
  char network_number[MAX],genbuf[MAX],host_bits[MAX],netmask[MAX];
  unsigned int bit_addr[4],mask_addr[4];
  if(argc < 2){
    usage(argv[0]);
    exit(1);
    }
  if(argc < 3){
    sprintf(genbuf,"%s/%s/%s.%s",PARKER_HOME,HOSTDATADIR,EDUCOL ,MASKFILEEXT);
  }else
    sprintf(genbuf,"%s",argv[2]); 
  if((fd=fopen(argv[1],"r"))&&(outfd=fopen(genbuf,"w")))
  { while(!feof(fd)){
     if(fscanf(fd,"%s%s%s",network_number,host_bits,netmask)!=EOF){
      sprintf(genbuf,"%s/%s\n",network_number,netmask);
      pptr1=host_bits; pptr2=netmask;
      for(i=0;i<4;i++){
        if((ptr1=strchr(pptr1,'.'))) *ptr1='\0';
        if((ptr2=strchr(pptr2,'.'))) *ptr2='\0';
        sscanf(pptr1,"%d",&bit_addr[i]);
        sscanf(pptr2,"%d",&mask_addr[i]);
        if(bit_addr[i]+mask_addr[i]!=255)
        { printf("mask defination wrong!\n");
          printf("network:%s\nbits:%s\nmask:%s\n",network_number,host_bits,netmask);
          return(0);
        }
      }
      fputs(genbuf,outfd);
     }
    }
  }
  return(0);
}

