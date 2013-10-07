/*
 *    NoseyParker, the search engine for FTP archives
 *    Copyright (C) 1993-96 by Jiri A. Randus 
 *                  2001-2002 by Steff
 *		    2004- by xxxss
 *		    
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *    The author can be reached as follows:
 *      Internet:   <Jiri.Randus@vslib.cz>
 *      Phone:      ++42 48 5227374
 *      SnailMail:  Jiri Randus
 *                  KIN HF TU v Liberci 
 *                  Halkova 6 
 *            46117 Liberec
 *                  Czech Republic
 *
 *    You can contact Steff by E-mail:mesolinux@263.net
 *    and contact xxxss by E-mail: xxxss@msn.com
 */

/*    This file was improved by mars, and now I change it to replace the perl.
 *    Thanks mars
 *                           ---Steff
 *                           2002.2.16
 *********************************************
 * Log:
 * 2004/04/19	Hai Xie - added more type of ftplist support
 * 2006/02/22   Sunry Chen - added URL 
*/

#include "parker.h"
#include "collect.h"

char ReplyCode[4];
char Line[MAX];
char WorkDir[MAX];
union InAddr
{
  long addr;
  char bytes[SIZEOFINETADDR];
}
myaddr;
struct filestatist
{
  int files;
  int rms;
  int mp3;
  int exefiles;
  int linuxfiles;
};
struct ftpsite site;
char machine[MAX];

FILE *CtlIn, *CtlOut;
struct sockaddr_in sa;

/* define the signal Alarm function */
void
Alarm (void)
{
  printf ("\007'%s' received ALARM !\n", machine);
  exit (101);
}

/* create a temp file */
char *
TempFile (char *dir, char *prfx)
{
  int fd;
  static char fn[MAX];
  sprintf (fn, "%s/%sXXXXXX", dir, prfx);
  if ((fd = mkstemp (fn)) == -1)
	  perror("mkstemp");
  close(fd);
  return (fn);
}


/* after called PWD, wait return code */
/* put the current path to *d */
int
GetPwd (char *d)
{
  char *ptr;
  fprintf (CtlOut, "PWD\r\n");
  while (1)
    {
      fgets (Line, MAX, CtlIn);
      if (feof (CtlIn))
	return (S_CLOSED);

#ifdef DEBUG
      printf ("%s", Line);

#endif /*  */
      if (Line[3] == '-')
	continue;
      if (!strncmp (Line, PWD, strlen (PWD)))
	{
	  ptr = Line;
	  while (ptr++)
	    if (*ptr == '\"')
	      break;

	    else if (*ptr == ':')
	      {
		ptr++;
		break;
	      }

	    else if (!*ptr)
	      return (S_ERROR);
	  strcpy (d, ptr + 1);
	  if ((ptr = strchr (d, '\"')))
	    *ptr = '\0';

	  else if ((ptr = strchr (d, ':')))
	    *(++ptr) = '\0';
	  DEB (d);
	  return (S_OK);
	}

      else if (*Line == NEGATIVE)
	return (S_ERROR);

      else if (*Line == PRENEGATIVE)
	return (S_ERROR);
    }
}


/* wait if success */
int
WaitSucc (void)
{
  while (1)
    {
      fgets (Line, MAX, CtlIn);
      if (feof (CtlIn))
	return (S_CLOSED);

#ifdef DEBUG
      printf ("%s", Line);

#endif /*  */
      if (Line[3] == '-')
	continue;
      if (!strncmp (Line, SUCCESS, strlen (SUCCESS)))
	return (S_OK);

      else if (*Line == PRENEGATIVE)
	return (S_ERROR);

      else if (*Line == NEGATIVE)
	return (S_ERROR);
    }
}


/* wait the reply from ftp server */
int
WaitReply (void)
{
  bzero (ReplyCode, sizeof (ReplyCode));
  while (1)
    {
      fgets (Line, MAX, CtlIn);
      if (feof (CtlIn))
	return (S_CLOSED);

#ifdef DEBUG
      printf ("%s", Line);

#endif /*  */
      if (Line[3] == '-')
	continue;
      ReplyCode[0] = Line[0];
      ReplyCode[1] = Line[1];
      ReplyCode[2] = Line[2];
      if (*Line == POSITIVE)
	return (S_OK);		/* POSITIVE = 2 */
      if (*Line == PROCEED)
	return (S_OK);		/* PROCEED = 3 */
      if (*Line == PREPOSITIVE)
	return (S_PREOK);	/* PREPREPOSIVE = 4 */

      else if (*Line == NEGATIVE)
	return (S_ERROR);	/* NEGATIVE = 5 */
    }
}
int
GetHostipStr (char *hostdns, char *hostip)
{
  int i;
  unsigned char IP[4];
  struct hostent *host;
  if (!(host = gethostbyname (hostdns)))
    {
      printf ("gethostbyname:%s!\n", hostdns);
      return (S_ERROR);
    }
  for (i = 0; i < 4; i++)
    IP[i] = (host->h_addr[i]);
  sprintf (hostip, "%d.%d.%d.%d", IP[0], IP[1], IP[2], IP[3]);
  return (S_OK);
}


/*  connect to the server ftp service */
/*  char *m is the ftp site address */
/*  and use CtlIn as income channel and CtlOut as outcome channel */
int
WaitCon (char *m)
{
  int h, a, port;
  struct hostent *host;
  struct sockaddr_in own_addr;
  socklen_t own_addr_len = sizeof (own_addr);
  if ((h = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
      perror ("socket");
      return (S_ERROR);
    }
  sa.sin_family = AF_INET;

//  sa.sin_port=htons(FTPPORT);
  port = atoi (site.ftp_port);
  sa.sin_port = htons (port);
  if (!(host = gethostbyname (m)))
    {

//      printf("gethostbyname:%s!\n",m);
      return (S_ERROR);
    }
  sa.sin_addr.s_addr = ((struct in_addr *) (host->h_addr))->s_addr;

/*  memcpy(&sa.sin_addr.s_addr,host->h_addr,SIZEOFINETADDR);*/
  if (connect (h, (void *) &sa, sizeof (sa)) < 0)
    {
      perror ("connect");
      return (S_ERROR);
    }
  if ((a = getsockname (h, (struct sockaddr *) &own_addr, &own_addr_len)))
    {
      perror ("getsockname");
      printf ("error here!return %i\n", a);
      return (S_ERROR);
    };
  myaddr.addr = own_addr.sin_addr.s_addr;

//  printf("my own IP is %u %u %u %u\n",(unsigned char)myaddr.bytes[0],(unsigned char)marsaddr.bytes[1],(unsigned char)marsaddr.bytes[2],(unsigned char)marsaddr.bytes[3]);
  if (CtlIn)
    fclose (CtlIn);
  if (CtlOut)
    fclose (CtlOut);
  if (!(CtlIn = fdopen (h, "r")))
    {
      perror ("fdopen(CtlIn)");
      exit (102);
    }
  if (!(CtlOut = fdopen (h, "w")))
    {
      perror ("fdopen(CtlOut)");
      exit (103);
    }
  setbuf (CtlIn, NULL);
  setbuf (CtlOut, NULL);
  return (S_OK);
}


/* replace "\" to "/" */
void
ConvertBS (char *p)
{
  while (*p)
    {
      if (*p == '\\')
	*p = '/';
      p++;
    }
}


/* connect to ftp server *p, use the following function */
/* call WaitCon then WaitReply */
/* use anonymous as User name */
/* then call GetPwd | ConvertBS to get current path */
int
Connect (char *p)
{
  if (WaitCon (p) != S_OK)
    return (S_ERROR);
  if (WaitReply () != S_OK)
    return (S_ERROR);

//  fprintf(CtlOut,"USER anonymous\r\n");
  fprintf (CtlOut, "USER %s\r\n", site.ftp_user);
  if (WaitReply () != S_OK)
    return (S_ERROR);
  if (!strcmp (ReplyCode, "331"))
    {

//      fprintf(CtlOut,"PASS %s\r\n",EMAILINFO);
      fprintf (CtlOut, "PASS %s\r\n", site.ftp_pass);
      if (WaitReply () != S_OK)
	return (S_ERROR);
    }
  DEB ("Logged in");
  return (S_OK);
}


/* judge if the string is month */
int
IsMonth (char *p)
{
  int i;
  static char *m[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug",
    "Sep", "Oct", "Nov", "Dec", NULL
  };
  for (i = 0; i < 12; i++)
    if (!strncasecmp (p, m[i], 3))
      return (S_OK);
  return (S_ERROR);
}


/* replace "/" to "\", add * to *s */
void
ConcStr (char *s, char *r)
{
  char *ptr, str[2];
  str[1] = 0;
  ptr = r;
  while (*ptr)
    {
      if (*ptr == '/')
	strcat (s, "\\");
      str[0] = *ptr;
      strcat (s, str);
      ptr++;
    }
}


/* Proceed dir list format */
int
DoFormat (char *topdir, char *infile, char *outfile,
	  struct filestatist *files)
{
  FILE *fin, *fout;
  char *ppdate = NULL, *ppsize = NULL, *ppfilename = NULL;
  char *ptr, *ppt;
  char unifn[MAX];
  int i;
  int IsDir = 0;
  DEB (outfile);
  files->exefiles = 0;
  files->files = 0;
  files->linuxfiles = 0;
  files->mp3 = 0;
  files->rms = 0;
  if (!(fin = fopen (infile, "r")))
    {
      perror ("fopen in error in DoFormat");
      return (S_ERROR);
    }
  if (!(fout = fopen (outfile, "w")))
    {
      perror ("fopen out error in DoFormat");
      return (S_ERROR);
    }
  fprintf (fout, "%s\n", topdir);
  while (!feof (fin))
    {
      fgets (Line, MAX, fin);
      if ((ptr = strchr (Line, CR)) != NULL)
	*ptr = '\0';
      if ((ptr = strchr (Line, LF)) != NULL)
	*ptr = '\0';
      if (!strcmp (Line, ".:"))
	continue;
      if (!strncmp (Line, "total ", 6))
	continue;
      ptr = Line;
      if (*ptr == '/')
	ptr++;
      if (!strncmp (ptr, "./", 2))
	ptr = ptr + 2;
      i = strlen (ptr);
      ppt = ptr + i - 1;
      if (*ppt == ':')
	{
	  if (*ptr == '.' && *(ptr + 1) == '\\')
	    ptr = ptr + 2;
	  *ppt = '/';
	  *(ppt + 1) = '\0';
	  ConvertBS (ptr);
	  i = 0;
	  while (ptr && i < MAX)
	    {
	      if (*ptr == ' ')
		{
		  strcpy (&unifn[i], "%20");
		  i = i + 3;
		  ptr++;
		}
	      else
		{
		  unifn[i] = *ptr;
		  i++;
		  ptr++;
		}
	    }
	  unifn[i] = '\0';
	  fprintf (fout, "%s%s\n", topdir, unifn);
	  continue;
	}
      if (Line[0] == 'l' && (strstr (Line, " -> ")) != NULL)
	{

	  /* unix link file handler */
	  continue;
	}

      else if (*Line <= '9' && *Line >= '0')
	{

	  /* DOS dir list handler */
	  /* format :
	     06-14-00  09:42PM                 2394 1.htm
	     06-14-00  09:42PM        <DIR>         DESKTOP
	   */
	  ppdate = Line;
	  ptr = strchr (Line, ' ');
	  if (!ptr)
	    continue;
	  while (*ptr == ' ')
	    if (*ptr == '\0')
	      continue;

	    else
	      *ptr++ = '_';
	  ptr = strchr (ptr, ' ');
	  if (!ptr)
	    continue;
	  *ptr++ = '\0';
	  while (*ptr && *ptr == ' ')
	    ptr++;
	  if (*ptr == '\0')
	    continue;
	  if (!strncmp (ptr, "<DIR>", 5))
	    {
	      IsDir = 1;
	      ppsize = NULL;
	    }
	  else
	    {
	      IsDir = 0;
	      ppsize = ptr;
	    }
	  ptr = strchr (ptr + 1, ' ');
	  if (!ptr)
	    continue;
	  *ptr++ = '\0';
	  while (*ptr && *ptr == ' ')
	    ptr++;
	  if (*ptr == '\0')
	    continue;
	  ppfilename = ptr;
	}

      else if (*Line == 'd' || *Line == '-')
	{

	  /* unix dir list handler */
	  /* format :
	     -rwxrw-rw- 1 root  root   423421 Aug  7  1999 setup.exe
	     drwxrw-rw- 1 root  root        0 Aug 22 15:34 DIR
	   */
	  if (*Line == 'd')
	    IsDir = 1;

	  else
	    IsDir = 0;
	  ptr = Line;
	  for (i = 0; i < 4 && ptr; i++)
	    {
	      ptr = strchr (ptr, ' ');
	      if (!ptr)
		continue;
	      while (*ptr == ' ')
		*ptr++ = '\0';
	    }
	  if (!ptr)
	    continue;
	  ppsize = ptr;
	  ptr = strchr (ptr, ' ');
	  if (!ptr)
	    continue;
	  while (*ptr == ' ')
	    *ptr++ = '\0';
	  ppdate = ptr;
	  for (i = 0; i < 2 && ptr; i++)
	    {
	      ptr = strchr (ptr, ' ');
	      if (!ptr)
		continue;
	      while (*ptr == ' ')
		*ptr++ = '_';
	    }
	  ptr = strchr (ptr, ' ');
	  if (!ptr)
	    continue;
	  while (*ptr == ' ')
	    ptr++;
	  *(ptr - 1) = '\0';
	  ppfilename = ptr;
	  if ((ptr = strchr (ppfilename, '*')))
	    *ptr = '\0';
	}
      else
	continue;
      if (strstr (ppfilename, ".RM") || strstr (ppfilename, ".rm")
	  || strstr (ppfilename, ".mov")
	  || strstr (ppfilename, ".avi")
	  || strstr (ppfilename, ".AVI")
	  || strstr (ppfilename, ".mpg")
	  || strstr (ppfilename, ".wmv") || strstr (ppfilename, ".MOV"))
	files->rms++;

      else if (strstr (ppfilename, ".MP3")
	       || strstr (ppfilename, ".mp3")
	       || strstr (ppfilename, ".wma")
	       || strstr (ppfilename, ".WMA")
	       || strstr (ppfilename, ".mpga") || strstr (ppfilename, ".mid"))
	files->mp3++;

      else if (strstr (ppfilename, ".EXE")
	       || strstr (ppfilename, ".exe")
	       || strstr (ppfilename, ".zip")
	       || strstr (ppfilename, ".rar") || strstr (ppfilename, ".ZIP"))
	files->exefiles++;

      else if (strstr (ppfilename, ".rpm")
	       || strstr (ppfilename, ".deb")
	       || strstr (ppfilename, ".RPM")
	       || strstr (ppfilename, ".tar")
	       || strstr (ppfilename, ".bz2")
	       || strstr (ppfilename, ".tgz") || strstr (ppfilename, ".gz"))
	files->linuxfiles++;
      files->files++;
      i = 0;
      ptr = ppfilename;
      while (ptr && i < MAX)
	{
	  if (*ptr == ' ')
	    {
	      strcpy (&unifn[i], "%20");
	      i = i + 3;
	      ptr++;
	    }
	  else
	    {
	      unifn[i] = *ptr;
	      i++;
	      ptr++;
	    }
	  if (*ptr == '\0')
	    break;
	}
      if (IsDir != 0)
	{
	  unifn[i] = '/';
	  i++;
	}
      unifn[i] = '\0';
      if (IsDir)
	fprintf (fout, "%s 0 %s\n", unifn, ppdate);

      else
	fprintf (fout, "%s %s %s\n", unifn, ppsize, ppdate);
      continue;
    }
  fclose (fout);
  fclose (fin);
  return (files->files);
}


/* called by DoLISTlR,DRecursive,DoDownload */
/* open a socket to receive ftp data */
/* tell the ftp server the port, Wait Reply */
/* if s hortlong =  */
/* = 0  LIST */
/* = 1  LIST -lR */
/* = 2  TYPE I, RETR *fn, to get list file *fn */
/* put the data to file *outfile */
int
QueryDir (char *outfile, char fetchType, char *outfilename)
{
  struct sockaddr_in da;
  int foo;
  int hh;
  int rc;
  int i, s, ns;
  int Port;
  FILE *in, *out;
  char *ptr;
  da = sa;
  da.sin_addr.s_addr = myaddr.addr;
  da.sin_port = 0;
  s = socket (AF_INET, SOCK_STREAM, 0);
  if (s < 0)
    {
      perror ("socket");
      return (S_ERROR);
    }
  foo = sizeof (da);
  while (bind (s, (struct sockaddr *) &da, foo) < 0)
    {
      perror ("bind");
      return (S_ERROR);
    }
  if (getsockname (s, (struct sockaddr *) &da, &foo) < 0)
    {
      perror ("ftp: getsockname");
      return (S_ERROR);
    }
  Port = ntohs (da.sin_port);
  if (listen (s, 1) < 0)
    {
      perror ("listen");
      return (S_ERROR);
    }
  fprintf (CtlOut, "PORT %u,%u,%u,%u,%u,%u\r\n",
	   (unsigned char) myaddr.bytes[0],
	   (unsigned char) myaddr.bytes[1],
	   (unsigned char) myaddr.bytes[2],
	   (unsigned char) myaddr.bytes[3], Port / 256,
	   Port - (unsigned int) (Port / 256) * 256);
  if ((rc = WaitReply ()) == S_OK)
    {
      switch (fetchType)
	{
	case 0:
	  fprintf (CtlOut, "LIST\r\n");
	  break;
	case 1:
	  fprintf (CtlOut, "LIST -lR\r\n");
	  break;
	case 2:
	  fprintf (CtlOut, "TYPE I\r\n");
	  WaitReply ();
	  fprintf (CtlOut, "RETR %s\r\n", outfilename);
	  break;
	default:
	  puts ("Wrong kind of `fetchType' argument in QueryDir");
	  return (S_ERROR);
	}
      if (WaitReply () != S_PREOK)
	return (S_ERROR_INFIMA);
      if ((hh = creat (outfile, MODE)) == -1)
	return (S_ERROR);
      foo = sizeof da;
      ns = accept (s, (struct sockaddr *) &da, &foo);
      if (ns < 0)
	{
	  perror ("accept");
	  return (S_ERROR);
	}
      close (s);
      if (fetchType == 2)
	while (1)
	  {
	    i = read (ns, Line, MAX);
	    if (!i)
	      break;
	    alarm (TIMEOUT);
	    write (hh, Line, i);
	  }

      else
	{
	  if (!(in = fdopen (ns, "r")))
	    {
	      perror ("fdopen in here");
	      exit (0);
	      return (S_ERROR);
	    }
	  if (!(out = fdopen (hh, "w")))
	    {
	      perror ("fdopen out");
	      return (S_ERROR);
	    }
	  while (1)
	    {
	      fgets (Line, MAX, in);
	      if (feof (in))
		break;
	      if ((ptr = strchr (Line, CR)) != NULL)
		*ptr = '\0';
	      if ((ptr = strchr (Line, LF)) != NULL)
		*ptr = '\0';
	      fprintf (out, "%s\n", Line);
	      alarm (TIMEOUT);
	    }
	  fclose (out);
	  fclose (in);
	}
      DEB ("Done with fetching the directory");
      close (ns);
      close (hh);
      rc = WaitSucc ();
    }
  return (rc);
}


/* use QueryDir(use LIST -lR) to get entire dir list */
int
DoLISTlR (char *outfile)
{
  int rc;
  fprintf (CtlOut, "CWD %s\r\n", "/");
  rc = WaitReply ();
  if (rc == S_CLOSED)
    {
      DEB ("Connection closed!!!");
      return (S_ERROR);
    }
  rc = QueryDir (outfile, 1, NULL);
  DEB ("Done with List-lR");
  return (rc);
}


/* use recursive LIST to get the entire directory list */
int
DoRecursive (char *topdir, FILE * res)
{
  char *ptr;
  char tmpfn[MAX], tmpstr[MAX];
  FILE *dirf;
  int rc, i;
  fprintf (CtlOut, "CWD %s\r\n", topdir);
  rc = WaitReply ();
  if (rc == S_CLOSED)
    {
      DEB ("Connection closed!!!");
      return (S_ERROR);
    }
  GetPwd (tmpstr);
  if (strcmp (topdir, tmpstr))
    return (S_ERROR);
  ConvertBS (topdir);
  strcpy (tmpfn, TempFile (WorkDir, "Dir"));
  rc = QueryDir (tmpfn, 0, NULL);
  if (rc != S_ERROR_INFIMA)
    printf ("%s:%s\n", machine, topdir);

  else
    {
      rc = S_OK;
      printf ("%s:%s - Major BBS Bug\n", machine, topdir);
    }
  if (!(dirf = fopen (tmpfn, "r")))
    {
      unlink (tmpfn);
      return (S_ERROR);
    }
  if (strcmp (topdir, "/"))
    fprintf (res, "%s:\n", topdir);
  while (1)
    {
      fgets (Line, MAX, dirf);
      if (feof (dirf))
	break;
      fputs (Line, res);
    }
  rewind (dirf);
  while (rc == S_OK)
    {
      fgets (Line, MAX, dirf);
      if (feof (dirf))
	{
	  rc = S_OK;
	  break;
	}
      if ((ptr = strchr (Line, CR)) != NULL)
	*ptr = '\0';
      if ((ptr = strchr (Line, LF)) != NULL)
	*ptr = '\0';
      if (1)
	{
	  DEB (Line);
	  if (*Line == 'l' && strstr (Line, " -> "))
	    continue;
	  if (*Line == 'd')
	    {
	      i = 1;
	      ptr = Line;
	      while (i < 9 && ptr)
		{
		  ptr = strchr (ptr, ' ');
		  if (ptr)
		    {
		      while (*ptr == ' ' && ptr)
			{
			  ptr++;
			}
		    }
		  i++;
		}
	    }
	  else if ((ptr = strstr (Line, "<DIR>")))
	    {
	      ptr = ptr + 5;
	      while (*ptr == ' ' && ptr)
		ptr++;
	    }

	  else
	    continue;
	  strcpy (tmpstr, topdir);
	  if (tmpstr[strlen (tmpstr) - 1] != '/')
	    strcat (tmpstr, "/");
	  strcat (tmpstr, ptr);
	  DEB (tmpstr);
	  rc = DoRecursive (tmpstr, res);
	  if (rc != S_OK)
	    {
	      DEB ("DoLookup returned an error");
	      rc = 0;
	    }
	}
    }
  fclose (dirf);
  unlink (tmpfn);
  return (rc);
}


/* use QueryDir(download the list file like list-lR.gz ) to get entire dir list */
int
DoDownload (char *outfile, char *topdir)
{
  char newdir[MAX];
  int i;
  char tmpfn[MAX];
  int rc;
  char command[MAX];
  struct path
  {
    char *path, *file;
  };
  struct path Lists[7];
  Lists[0] = (struct path)
  {
  "/", "ls-lR.gz"};
  Lists[1] = (struct path)
  {
  "/", "ls-lR.Z"};
  Lists[2] = (struct path)
  {
  "/", "ls-lR"};
  Lists[3] = (struct path)
  {
  "/pub/", "ls-lR.gz"};
  Lists[4] = (struct path)
  {
  "/pub/", "ls-lR.Z"};
  Lists[5] = (struct path)
  {
  "/pub/", "ls-lR"};
  Lists[6] = (struct path)
  {
  NULL, NULL};
  rc = 0;
  strcpy (tmpfn, TempFile (WorkDir, "Down"));
  for (i = 0; Lists[i].path; i++)
    {
      fprintf (CtlOut, "CWD %s\r\n", Lists[i].path);
      rc = WaitReply ();
      if (rc == S_CLOSED)
	{
	  DEB ("Connection closed!!!");
	  exit (111);
	}

      else if (rc != S_OK)
	continue;
      GetPwd (newdir);
      ConvertBS (newdir);
      if ((rc = QueryDir (tmpfn, 2, Lists[i].file)) == S_OK)
	break;
    }
  if (Lists[i].path)
    {
      sprintf (command, "%s:%s Fetched %s%s\n", machine, topdir,
	       Lists[i].path, Lists[i].file);
      DEB (command);
      if (toupper (Lists[i].file[strlen (Lists[i].file) - 1]) == 'Z')
	{
	  DEB ("Uncompressing");
	  strcpy (command, MV);
	  strcat (command, " ");
	  strcat (command, tmpfn);
	  strcat (command, " ");
	  strcat (command, outfile);
	  strcat (command, ".gz");
	  system (command);
	  strcpy (command, GUNZIP);
	  strcat (command, " ");
	  strcat (command, outfile);
	  system (command);
	}
      else
	{
	  strcpy (command, MV);
	  strcat (command, " ");
	  strcat (command, tmpfn);
	  strcat (command, " ");
	  strcat (command, outfile);
	  system (command);
	}
    }

  else
    {
      DEB ("Giving up remote ls-lR files...");
      unlink (tmpfn);
    }
  return (rc);
}


/* use all sollution to get the dir list */
/* or use the certain function */
/* TypeOfFetch=r call DoRec */
/* TypeOfFetch=l call DoLookup */
/* else call DolslR or DoLookup*/
int
Probe (char *outputdir, char TypeOfFetch)
{
  int rc;			/* return code */
  FILE *res = NULL;
  char tempfile[MAX];
  char resfile[MAX];
  char command[MAX];
  char TopDir[MAX];		/* the ftp top dir */
  char HostIP[MAX];
  char URL[MAX];
  struct filestatist files;
  GetHostipStr (machine, HostIP);
  sprintf (resfile, "%s/%s.org", WorkDir, HostIP);
  DEB ("Probe");
  alarm (TIMEOUT);
  if (Connect (machine) != S_OK)
    {
      DEB ("Couldn't connect");
      return (S_ERROR);
    }
  strcpy (TopDir, "/");
  if (TypeOfFetch == 'd')
    {
      rc = DoDownload (resfile, TopDir);
    }
  else if (TypeOfFetch == 'l')
    {
      rc = DoLISTlR (resfile);
    }
  else if (TypeOfFetch == 'r')
    {
      res = fopen (resfile, "w");
      rc = DoRecursive (TopDir, res);
      fclose (res);
    }
  else
    if ((rc = DoLISTlR (resfile)) != S_OK
	&& ((rc = DoDownload (resfile, TopDir)) != S_OK))
    {
      DEB ("use Recursive Look up");
      res = fopen (resfile, "w");
      if ((rc = DoRecursive (TopDir, res)) != S_OK)
	return (S_ERROR);
      fclose (res);
    }
  if (rc != S_OK)
    return (S_ERROR);
  alarm (0);
  sprintf (tempfile, "%s/%s.fmt", WorkDir, HostIP);
  DoFormat (TopDir, resfile, tempfile, &files);

// For debug usage.
// printf("port: %s\n user: %s\n pass: %s\n name: %s\n hostip: %s\n",site.ftp_port,site.ftp_user,site.ftp_pass,site.ftp_name,
// HostIP);
// exit(0);
//patched by Sunry Chen 2006-02-22
  if (strcasecmp(site.ftp_user,"ANONYMOUS"))
  {
    strcat(URL,site.ftp_user);
    strcat(URL,":");
    strcat(URL,site.ftp_pass);
    strcat(URL,"@");
  }
  strcat(URL,HostIP);
  printf("--- URL: %s\n",URL);

  sprintf (command, "%s/%s/%s %s %s/%s -s", PARKER_HOME, BINDIR,
//	   LZOCOMP, tempfile, outputdir, HostIP);
	LZOCOMP, tempfile, outputdir, URL);
  system (command);
  unlink (resfile);
  unlink (tempfile);
  sprintf (resfile, "%s/%s/%s.%s", PARKER_HOME, HOSTDATADIR, HostIP,
	   HOSTINFOEXT);
  res = fopen (resfile, "w");
  fprintf (res, "%s %s %d %d %d %d %d\n", machine, HostIP,
	   files.files, files.rms, files.mp3, files.exefiles,
	   files.linuxfiles);
  fclose (res);
  DEB ("Done with Probe");
  return (rc);
}


/* scan the ftp site, support formats below:
 * ftp://username:passwd@hostip:port
 * ftp://username:passwd@hostip
 * ftp://username@hostip:port
 * ftp://username@hostip
 * ftp://hostip:port
 * ftp://hostip
 * the string "ftp://" is dispensable
 * use "anonymous" when no username;
 * use "my@my.com" when no passwd;
 * use 21 when no port;
 * hostdn or hostip must have;
 *
 * added by xxxss 2004-04-19
 */

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
//    strcpy (site.ftp_user, ANONYMOUS);
//  if (site.ftp_pass[0] == '\0')
  {
    strcpy (site.ftp_user, ANONYMOUS);
    strcpy (site.ftp_pass, ANONYPASS);
  }
}

/* use Probe to get the dir list */
int
main (int n, char *p[])
{
  char LZODir[MAX];
  char TypeOfFetch = ' ';
  int child;
  int hostnum;
  FILE *hostListRes;
  if (n > 3)
    {
      printf ("USAGE: collect\n");
      printf ("       collect [-d|-l|-r] [destination ftp host]\n");
      printf ("      -d download the list file like ls-lR.tgz \n");
      printf ("      -l use \"LIST -lR\" \n");
      printf ("      -r use recursive \"LIST\" to get the entire dir \n");
      return (0);
    }
  nice (PARKER_NICE);
  signal (SIGALRM, (void (*)(int)) Alarm);
  sprintf (WorkDir, "%s/%s", PARKER_HOME, WORKDIR);
  sprintf (LZODir, "%s/%s", PARKER_HOME, LZOFILEDATADIR);
  hostnum = 0;
  if (n > 1 && *p[1] == '-')
    TypeOfFetch = p[1][1];
  if (n == 3 || (n == 2 && *p[1] != '-'))
    {
      if (n == 3)
	strcpy (machine, p[2]);

      else
	strcpy (machine, p[1]);
      strcpy (site.ftp_port, "21");
      strcpy (site.ftp_user, ANONYMOUS);
      strcpy (site.ftp_pass, ANONYPASS);
      if (Probe (LZODir, TypeOfFetch) != S_OK)
	printf ("Cannot get the ftp data on host  %s.\n", machine);
    }

  else
    {				/*  use default collect host list */
      strcpy (Line, HOSTLIST);
      if (!(hostListRes = fopen (Line, "r")))
	{
	  printf ("Can't open Collect list file: %s\n", Line);
	  return 0;
	}
      while (1)
	{
	  fgets (Line, MAX, hostListRes);
	  if (feof (hostListRes))
	    break;
	  while ((child = fork ()) == -1)
	    sleep (1);
	  if (!child)
	    {
//                              strcpy(machine, Line);
	      scanline (Line);
	      Probe (LZODir, TypeOfFetch);
	      exit (0);
	    }

	  else
	    {
	      hostnum++;
	      sleep (5);
	      if (hostnum >= MAXAGENTS)
		{
		  wait (&child);
		  hostnum--;
		}
	    }
	}
      fclose (hostListRes);
      while (hostnum--)
	wait (&child);
    }
  return 0;
}
