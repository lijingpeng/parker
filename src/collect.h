/*
 *    NoseyParker, the search engine for FTP archives
 *    Copyright (C) 1993-96 by Jiri A. Randus 
 *                  2001-2002 by Steff
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
 *    
 */

/*    This file was improved by mars, and now I change it to replace the perl.
 *    Thanks mars
 *                           ---Steff
 *                           2002.2.16
 */
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
#include <netinet/in.h>
#include <arpa/inet.h>


#define DEBUG  
#ifndef DEBUG
#define DEB(xxx)        {}
#define MAXAGENTS       5
#else
#define DEB(xxx)        {printf("--- %s\n",xxx);fflush(stdout);}
#define MAXAGENTS       1
#endif

#define EMAILINFO	"xxxss@msn.com"

#define PREPOSITIVE	'1'
#define POSITIVE	'2'
#define PROCEED         '3'
#define PRENEGATIVE	'4'
#define NEGATIVE	'5'
#define SUCCESS         "226 "
#define PWD             "257 "



#define SIZEOFINETADDR	4
#define TIMEOUT         40

#define MINLENGTHOFSEARCH 3

#define S_PREOK		1
#define S_OK		0
#define S_ERROR		-1
#define S_CLOSED	-2
#define S_ERROR_INFIMA	-3 /* A fix to the Major BBS's RFC 959 LIST violation */

#define PARKER_NICE     20

#define CGIALARM        600

#define FTPPORT         21
#define MODE            0644
#define TIMEOUT         1200

#define CR 13
#define LF 10




#define LZOCOMP "lzo_comp"
#define MV  "mv"
#define GUNZIP "gunzip"
#define ANONYMOUS "anonymous"
#define ANONYPASS "my@my.com"
