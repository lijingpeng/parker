#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "minilzo.h"

#define MAX 1024

#define MAXBLOCKSIZE (256*1024L)
#define BUF_LEN		(MAX + 256*1024L)
#define PLZOMARK "Parker LZO compressed data"
#define BLOCKMARK "BB"

FILE *fi;
int f_eof=0;
void *buf, *workmem; 
void *cur_ptr ;

unsigned long buf_len, reserved=MAX;
lzo_uint org_fsize=0, org_lines=0, data_len, block_size;

int lzo_gets(char *string)
{ 
	int i=0,r=0;
	char *in,*out;
	char mark[sizeof(BLOCKMARK)];
	lzo_uint com_len, new_len;
	unsigned long buf_left;

	buf_left = (buf - cur_ptr) + data_len;
	if( buf_left < reserved && !f_eof)
	{
		if (fread(mark,1,sizeof(BLOCKMARK)-1,fi) != sizeof(BLOCKMARK)-1 || memcmp(mark,BLOCKMARK,sizeof(BLOCKMARK)-1) != 0)
	        {
                	printf("block mark error.\n");
			fflush(stdout);
			goto err;
		}

		/* read uncompressed size */
		fread(&com_len,1,sizeof(lzo_uint),fi);
		if(feof(fi)){
			f_eof=1;
			goto end;
		}
		/* exit if last block (EOF marker) */
		if (com_len == 0)
		{	
			f_eof=1;
			goto end;
		}

		/* read compressed size */
		fread(&data_len,1,sizeof(lzo_uint),fi);
		if(feof(fi) || data_len == 0 || data_len < com_len)
		{	f_eof=1;
			goto end;
		}

		/* sanity check of the size values */
		if (com_len > block_size || data_len > block_size || com_len == 0 )
		{
			/* block size error - data corrupted */
			printf("length error!com=%lu,data=%lu\n",(unsigned long)com_len,(unsigned long)data_len);
			fflush(stdout);
			goto err;
		}

		/* place compressed block at the top of the buffer */
		if(buf_left > 0){
			memcpy(buf, cur_ptr, buf_left);
			cur_ptr = buf;
		}
 		in = buf + buf_left + buf_len - com_len;
 		out = buf + buf_left;

		/* read compressed block data */
		new_len=fread(in,1,(size_t)com_len,fi);
		if(new_len !=com_len) {
                        printf("length error!scripted=%lu,real=%lu\n",(unsigned long)com_len,(unsigned long)new_len);

			puts("compress block length error!\n");
			goto err;
		}
		
		if (com_len < data_len)
		{
			/* decompress - use safe decompressor as data might be corrupted */
			r = lzo1x_decompress(in,(lzo_uint )com_len,out,(lzo_uint *)&new_len,NULL);
                        if (r != LZO_E_OK )
                        {
                                puts("uncompress error!\n");
                                fflush(stdout);
                                goto err;
                        }

			if ( new_len != data_len)
			{
				/* compressed data violation */
				printf("new_len=%lu,data_len=%lu",(unsigned long)new_len,(unsigned long)data_len);
				puts("data block size did not == scripted\n");
				fflush(stdout);
				goto err;

			}
		}
		else
		{
			puts("len error: com_len >= data_len");
			fflush(stdout);
			goto err;
		}
		buf_left = buf + data_len - cur_ptr;
	}
end:
	for(i=0;i<buf_left && i < MAX -1;i++,cur_ptr++)
	{
		 *(string+i)= *(char *)cur_ptr;
		if( (char)*(string+i)=='\n'){
			cur_ptr++;
			*(string+i )='\0';
			break;
		}
	}

	return(i);
err:
	printf("some thing wrong in lzo_gets.\n");
	fflush(stdout);
	return -1;
}

int lzoInit(void)
{
    	if (lzo_init() != LZO_E_OK)
    	{
        	printf("lzo_init() failed !!!\n");
        	return(0);
    	}
	buf_len = BUF_LEN;
	if((buf = malloc(buf_len)) ){
		return(1);
	}
	else
		return(0);
}
FILE *lzo_fopen(const char *filename)
{
	char m[ sizeof(PLZOMARK) ];

	if(!(fi=fopen(filename,"rb"))){
		puts("file open error");
		goto err;
	}
	f_eof=0;
 	if (fread(m,1,sizeof(PLZOMARK)-1,fi) != sizeof(PLZOMARK)-1 || memcmp(m, PLZOMARK, sizeof(PLZOMARK)-1) != 0)
	{
		printf("not a parker lzo file.\n");
/*		*(m+sizeof(PLZOMARK))='\0';
		puts("<br>\n");puts(m);
		puts("<br>\n");puts(PLZOMARK);
*/		puts("<br>");
		goto err;
	}
	fread(&org_fsize,1,4,fi);
	fread(&org_lines,1,4,fi);
	fread(&block_size,1,4,fi);
	if (block_size < 1024 || block_size > MAXBLOCKSIZE)
	{
		/* header error - invalid block size */
		printf("block size error");
		goto err;
	}
	cur_ptr = buf;
	data_len=0;
	return(fi);
err:
	return(NULL);
}

void lzo_fclose(void)
{
	if(fi)	fclose(fi);
}
void lzo_close(void)
{
	if(buf) free(buf);
}
