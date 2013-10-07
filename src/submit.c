/************************************************/
/*  submit.c cgi version 4.0			*/
/*  this file is written for submit             */
/*                          			*/
/*  author steff	2001/5/4                */
/************************************************/
#include "parker.h"

void putHead(void)
{
  puts("Content-type:text/html\n\n");
  puts("<html><head><TITLE>FTP��վ�Ƽ�</TITLE></head>\n");
  puts("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=gb2312\">\n");
  puts("<BODY BGCOLOR=#ffffff background='/images/statbg.gif' align=left>\n");
  puts("������������ĸ�ftpվ��ȽϺã���soso��������û�а������վ�㣬<br>\n");
  puts("��ô�������������ύ�Ǹ�ftp�ĵ�ַ��<br>\n");
}
void putPage(void)
{
  puts("<table width=\"50\" border=\"0\" cellspacing=\"1\" cellpadding=\"1\" align=\"center\">\n");
  puts("    <tr align =\"center\">  \n");
  puts("      <td  valign=\"top\" >  \n");
  puts("        <FORM ACTION=\"/cgi-bin/parker/submit\" name=\"ftp_submit\"> \n");
  puts("          �Ƽ�FTP��վ<INPUT TYPE=\"text\" NAME=\"ftp\" SIZE=\"20\"><BR> \n");
  puts("          ����E-mail <INPUT TYPE=\"text\" NAME=\"email\" SIZE=\"20\"><BR> \n");
  puts("	  <INPUT  type=submit style=\"FONT-SIZE: 15px\" value=\"�ύ\"> \n");
  puts("	</FORM><br> \n");
  puts("      </td> \n");
  puts("    </tr> \n");
  puts("  </table> \n");
  fflush(stdout);
}
void putEnd(void)
{
  puts("<div><dir><H4><a href=\"/\">������ҳ</a></H4>");
  puts("</body>\n");
  puts("<script  language='JavaScript'>\n");
  puts("function Submit()\n");
  puts("{ if (document.ftp_submit.ftp.value==\"\"){\n");
  puts("	alert(\"����дFTP��վ��ַ!\");\n");
  puts("	return false;	}\n");
  puts("  else{	document.location=\"/cgi-bin/parker/submit?ftp=\"+document.ftp_submit.ftp.value+\"&email=\"+document.ftp_submit.email.value;  }\n");
  puts("} \n");
  puts(" \n");
  puts("</SCRIPT></html>\n");
  fflush(stdout);
}

void showthank(char *dnsname,char *ipstring,int ipclass,int inlib)
{
  puts("<div align=center><H1>��л���Ա�վ��֧��</H1></div>");
  puts("<table align='center' width=400><tr align='center' ><tb align='center'>");
  if(ipclass==1){ /* in nju school */
    printf("���Ƽ���FTPվΪУ�ڵ���վ��<br>\n");
  }else if(ipclass==2){ /* It is edu.cn free ip */
    printf("���Ƽ���FTPվΪ�й��������ڵ���Ʒ���վ��<br>\n");
  }else if(ipclass==3){/* out of country */
    printf("���Ƽ���FTPվΪ������վ����ΪĿǰ�����Ʒѵ�ԭ����ʱ�޷����ӡ�<br>\n");
    puts("һ��Ѱ�ҵ����ӵķ��������Ǿͽ���������������С�<br>");
    return;
  }else if(ipclass==-1){/* out of country */
    printf("���Ƽ���FTPվΪ����У���Զ���IP��<br>\n");
  }else{
    printf("���Ƽ���FTPվ%s�޷��������������ƴд��<br>",dnsname);
    return;
  }
  printf("ipΪ<b>%s</b>��<br>����Ϊ<b>%s</b><br>\n",ipstring,dnsname);
  if(inlib)
    printf("Ŀǰ�Ѿ���¼�ڱ�վ�����б�<br>\n");
  else
    printf("�����Ѿ����뵽��վ�������б�<br>20���Ӻ�����ѵ���վ������.\n");
  puts("�ٴθ�л���Ա�վ��֧�֡�</td></tr></table>");
}

int main()
{
  char *query,*pptr;
  char ftphost[MAX],email[MAX],dnsname[MAX],ipstring[20],genbuf[MAX];
  unsigned char addr[4];
  int ret=0,ipclass=0,inlib=0;

  alarm(CGIALARM);
  query=getenv(QUERYSTRING);
  putHead();
  strcpy(ftphost,"");
  if(query && *query ){
      while(query && *query){
        if(!strncmp(query,"ftp=",4)) {
          query+=4;
          if((pptr=strchr(query,'&'))!=NULL) *pptr='\0'; /* set the NULL at end of string */
          strncpy(ftphost,query,MAX-1);
	*(ftphost+MAX-1)='\0';
        } else if(!strncmp(query,"email=",6)) {
          query+=6;
          if((pptr=strchr(query,'&'))!=NULL) *pptr='\0'; /* set the NULL at end of string */
          strncpy(email,query,MAX-1);
	*(email+MAX-1)='\0';
        }
        query=strchr(query,'&');
        if(query) query++;
      }
    if(strlen(ftphost)){
     ret=getftpname(ftphost,dnsname,addr);
     if(ret>-2){
      sprintf(ipstring,"%d.%d.%d.%d",addr[0],addr[1],addr[2],addr[3]); 
      ipclass=getclass(addr);
      if(ipclass<=2){ /* free IP, can search */
        inlib=addftp(ipstring);
      }
      if(ret<3)/* fail on getting DNS name */
        strcpy(dnsname,ftphost);
    }else{
      ipclass=4; /* out of CERNET */
      strcpy(dnsname,ftphost);
    }
    showthank(dnsname,ipstring,ipclass,inlib);
   }
  }
  putPage();
  putEnd();
  /* log submit */
  sprintf(genbuf,"E-Mail:%s, Submit:'%s',dns:%s,ip:%s,class:%d",email,ftphost,dnsname,ipstring,ipclass);
  Log(SUBMITLOGFILE,genbuf);
  return(0);
}

