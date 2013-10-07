/************************************************/
/*  parker.h     version 4.2			*/
/*  this file is the head for all function      */
/*                          			*/
/*  improved from Noisy Parker                	*/
/*  author steff	2002/2/16                */
/************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <dirent.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>

#include "config.h"

/*#define DEBUG   */
#define USE_LZO

#define MINLENGTHOFSEARCH 2
#define MAX             1024
#define MAXSTR		5
#define CGIALARM        600
#define QUERYSTRING	"QUERY_STRING"
#define ACCEPT_LANGUAGE 	"HTTP_ACCEPT_LANGUAGE"


#define CR 13
#define LF 10

/* search type define */
#define SSUBSTRING	"substring"
#define SWILDCARDS	"wildcards"
#define SCASE		"case"
#define SINCASE		"incase"
/* define collection name */
#define LOCOL "Local"
#define EDUCOL "Edu"
#define ALLCOL "All"


/* define the arguments to struct */
struct StartPoint{
  int StartHost;
  int StartLine;
};

struct CgiArg
{ char String[MAX];
  char Not[MAX];
  char Ext[MAX];
  int  Hits;
  char SearchType[MAX]; /* sub=1, wild=2, egrep=3 */
  char Case[MAX];
/*  char Collection[MAX];  {School,Educn,All} */
  struct StartPoint StartPoint;
};

struct LanguageDef {
char *Language;
char *HHostName;
char *HHtmlCharset;
char *HHostURL;

char *HLeftQuation;
char *HRightQuation;
char *HSearchResult;
char *HHtmlTitle;
char *HUse;
char *HKeyword;
char *HToSearch;
char *HSubstring;
char *HCaption;
char *HCase;
char *HIsNotCase;
char *HExclude;
char *HExtNameIs;
char *HSearchFileNamed1;
char *HSearchFileNamed2;
char *HThisSearchResult;
char *HUpdata;
char *HBytes;
char *HSorryNothingFound;
char *HHaveList;
char *HHFiles;
char *HPressHere;
char *HGetMore;

char *HNewSearch;
char *HFullFileName;
char *HFileNameInclude;
char *HGoSearch;
char *HExclusion;
char *HExtName;
char *HSearchType;
char *HIsCaseCaption;
char *HIsNotCaseCaption;
char *HResultsOneScreen;
char *HProductby;

char *HYear;
char *HMonth;
char *HDay;

char *HFTPList;
char *HSite;
char *HArchiveNum;
char *HFilmNum;
char *HMusicNum;
char *HWinSoftNum;
char *HLinuxSoftNum;
char *HUpdataTime;
char *HLive;
char *HInfo;
};

struct ftpsite
{
  char ftp_user[128];
  char ftp_pass[128];
  char ftp_name[MAX];
  char ftp_port[10];
};


/* Externs */


int getftpname(char *input,char *dnsname,unsigned char *addr); /* get by net query */
int getdnsname(char *ip, char *dnsname); /* get from HostName/ip.host */
int getfiletime(char *host, char *timestr, struct LanguageDef *Langage);
int cmpfiletime(char *host);
int getclass(unsigned char *addr);
int string2addr(char *ipstring, unsigned char *addr);
void Log(char *, char *);
void onlyLog(char *logfilename, char *logstring);
int addftp(char *ipstring);

/* debug var */
#ifdef DEBUG
int showline;
#endif
