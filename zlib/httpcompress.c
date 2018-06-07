#include <zlib.h>
#include <stdlib.h>
#include <stdio.h>

// lmg_terminal.c在使用 2017-6-21
char * httpcompress(const char *buffer, int* length)
{
    unsigned char* _zipSrc;
    unsigned char* _zipDst;
    //unsigned char  _scBuffer[MaxLen];
    int zipLen;
	char *p = NULL;
	
    //先对原始内容进行压缩工作
    unsigned int tmpLen = strlen(buffer)+64;
    unsigned int sourceLen = strlen(buffer);
    printf("tmpLen=%d\n", tmpLen);
    _zipSrc = (unsigned char *)malloc(tmpLen+1);
    if (!_zipSrc)
    {
		printf("compress failed\n");
		return -1;
    }
    
    memset(_zipSrc, 0, tmpLen+1);
    
    _zipDst = (unsigned char *)malloc(tmpLen+1);
    if (!_zipDst)
    {
		printf("encodeZip malloc zipDst failed\n");
		free(_zipSrc);
		return -1;
    }
    memset(_zipDst, 0, tmpLen+1);
    memcpy(_zipSrc, buffer, sourceLen);
    
    z_stream c_stream;
    
    c_stream.zalloc = (alloc_func)0;
    c_stream.zfree = (free_func)0;
    c_stream.opaque = (voidpf)0;
    c_stream.next_in = (Bytef*)_zipSrc;
    c_stream.avail_in = sourceLen;
    
    int ret = deflateInit2(&c_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 31, 8, Z_DEFAULT_STRATEGY);
    
    if (ret != Z_OK)
    {
		printf("encodeZip deflateInit2 error %d\n",ret);
		free(_zipDst);
		free(_zipSrc);
		return -2;
    }
    
    do {
		memset(_zipDst, 0, tmpLen+1);
	    c_stream.next_out = (Bytef*)_zipDst;
	    c_stream.avail_out = (unsigned int)tmpLen;
	    ret = deflate(&c_stream, Z_FINISH);

		printf("encodeZip ret = %d, Z_STREAM_END=%d, Z_OK=%d, avail_out=%d, total_out=%d\n",
			ret, Z_STREAM_END, Z_OK, c_stream.avail_out, c_stream.total_out);

	    if ((ret != Z_STREAM_END && ret != Z_OK) || ret < 0)
	    {
		    break;
	    }
	    
    } while (c_stream.avail_out == 0);
    
    if (ret != Z_STREAM_END)
    {
		deflateEnd(&c_stream);

		printf("encodeZip ret != Z_STREAM_END err=%d\n", ret);

		free(_zipDst);
		free(_zipSrc);
		return -3;
    }
    
    zipLen = c_stream.total_out;
    
    ret = deflateEnd(&c_stream);
    
    if (ret != Z_OK)
    {
		printf("encodeZip deflateEnd error %d", ret);
		free(_zipDst);
		free(_zipSrc);
        return -4;
    }
    //压缩完毕进行返回包组织
    //memcpy(_scBuffer, _zipDst, zipLen);
    p = malloc(zipLen+1);
    memset(p, 0, zipLen+1);
    memcpy(p, _zipDst, zipLen);
    *length = zipLen;
    
    free(_zipDst);
    free(_zipSrc);
    
    return p;
}

int main(){
		int ziplen = 0;
		//char *buf = httpcompress("{ \"mac\": \"a8803822d6ae\", \"seqId\": 0, \"bssid\": [ \"a8803822d6ac\" ] }hello333333111", &ziplen);
		char *buf = httpcompress("{hello333333111ggggggggggggggggggggggggggggggggggg", &ziplen);
		
		FILE *fp = fopen("test.gz", "w");
		if (fp) {
			int len=fwrite(buf, ziplen, 1, fp);
			fflush(fp);
			fclose(fp);
			//printf("len=%d, ziplen=%d\n", len, ziplen);
		}
		
		
		
}
