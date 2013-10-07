/* testmini.c -- very simple test program for the miniLZO library

   This file is part of the LZO real-time data compression library.

   Copyright (C) 1996-2000 Markus Franz Xaver Johannes Oberhumer

   The LZO library is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   The LZO library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the LZO library; see the file COPYING.
   If not, write to the Free Software Foundation, Inc.,
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

   Markus F.X.J. Oberhumer
   markus.oberhumer@jk.uni-linz.ac.at
 */

#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/*************************************************************************
// This program shows the basic usage of the LZO library.
// We will compress a block of data and decompress again.
//
// For more information, documentation, example programs and other support
// files (like Makefiles and build scripts) please download the full LZO
// package from
//    http://wildsau.idv.uni-linz.ac.at/mfx/lzo.html
**************************************************************************/

/* First let's include "minizo.h". */

#include "minilzo.h"


/* We want to compress the data block at `in' with length `IN_LEN' to
 * the block at `out'. Because the input block may be incompressible,
 * we must provide a little more output space in case that compression
 * is not possible.
 */

#define IN_LEN		(128*1024L)
#define OUT_LEN		(IN_LEN + IN_LEN / 64 + 16 + 3)
#define WORK_LEN	(512*1024L)


/* Work-memory needed for compression. Allocate memory in units
 * of `long' (instead of `char') to make sure it is properly aligned.
 */


#define PLZOMARK "Parker LZO compressed data"
#define BLOCKMARK "BB"

/* file struct:
{ PLZOMARK;
  original file size: unsigned long;
  original file lines:unsigned long;
  maximun block size: unsigned long;
  {mark 'BB'
   compressed data block size:  unsigned long;
   orginal data size         :  unsigned long;
   data block                :
  }
  {
   ...
  }
  ...
  mark 'BB'
  unsigned long=0;(end mark)
}
*/

/*************************************************************************
//
**************************************************************************/

int main(int argc, char *argv[])
{
	int r=0;
	lzo_uint in_len, out_len, new_len;
	struct stat in_fstat;
    	lzo_uint org_fsize=0,org_lines=0,max_bsize=IN_LEN;
	void *com_cache;
	void *data_cache;
	void *workmem;
	FILE *fi,*fo;


	if (argc < 2){	/* avoid warning about unused args */
	printf("\nLZO real-time data compression library (v%s, %s).\n",
       lzo_version_string(), lzo_version_date());
	printf("Copyright (C) 1996-2000 Markus Franz Xaver Johannes Oberhumer\n\n");

		printf("use %s <input file> <output file> [-s]\n",argv[0]);
		return 0;
	}  
	stat(argv[1], &in_fstat);
	org_fsize=in_fstat.st_size;
	if(!(fi=fopen(argv[1],"rb"))){
		printf("open input file %s error!\n",argv[1]);
		return(2);
	}
	if(!(fo=fopen(argv[2],"wb"))){
		printf("creat output file error!\n");
		return(3);
	}
	if(!(data_cache=malloc(IN_LEN))){
		printf("malloc block space error!\n");
		return(4);
	}
    if(!(com_cache=malloc(OUT_LEN))){
        printf("malloc block space error!\n");
        return(4);
    }
    if(!(workmem=malloc(WORK_LEN))){
        printf("malloc block space error!\n");
        return(4);
    }
    if (lzo_init() != LZO_E_OK)
    {
        printf("lzo_init() failed !!!\n");
        return (5);
    }

	fwrite(PLZOMARK,sizeof(PLZOMARK)-1,1,fo);
	fwrite(&org_fsize,sizeof(org_fsize),1,fo);
    	fwrite(&org_lines,sizeof(org_lines),1,fo);
    	fwrite(&max_bsize,sizeof(max_bsize),1,fo);
	if(argc == 3)
		puts("begin to compress.");
	in_len=IN_LEN;
	while(!feof(fi)){
		new_len=fread(data_cache, 1, in_len, fi);
		if(new_len > 0)
			r = lzo1x_1_compress(data_cache, new_len, com_cache, &out_len, workmem);
		else
			break;
		if (r != LZO_E_OK  )
    		{
        		printf("internal error - compression failed: %d\n", r);
	        	return 4;
    		}
		if(argc==3)
        		printf("compressed %lu bytes into %lu bytes\n",(long) in_len, (long) out_len);
		fwrite(BLOCKMARK,2,1,fo);
		fwrite(&out_len,4,1,fo);
        	fwrite(&new_len,4,1,fo);
		fwrite(com_cache,(size_t) out_len, 1, fo);
	}
	fwrite(BLOCKMARK,2,1,fo);
	out_len=0;
	fwrite(&out_len,4,1,fo);
	fclose(fi);
	fclose(fo);
	free(com_cache);
	free(data_cache);
	free(workmem);
	return 0;
}


