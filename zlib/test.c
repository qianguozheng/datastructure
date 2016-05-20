#include <zlib.h>
//#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
using namespace std;

#define CHUNK	100000
#define MaxLen	100000
#define BUF_SIZE 65535

int decodeZip(char *source,int len,char **dest)
{
    int ret; 
    unsigned have; 
    z_stream strm; 
    unsigned char out[CHUNK]; 
    int totalsize = 0; 
    
    /* allocate inflate state */ 
    strm.zalloc = Z_NULL; 
    strm.zfree = Z_NULL; 
    strm.opaque = Z_NULL; 
    strm.avail_in = 0; 
    strm.next_in = Z_NULL; 

    ret = inflateInit2(&strm, 31); 
    
    if (ret != Z_OK) 
        return ret; 
    
    strm.avail_in = len; 
    strm.next_in = (unsigned char*)source; 
    
    /* run inflate() on input until output buffer not full */ 
    do { 
        strm.avail_out = CHUNK; 
        strm.next_out = out; 
        ret = inflate(&strm, Z_NO_FLUSH); 
        switch (ret) 
        { 
            case Z_NEED_DICT: 
                ret = Z_DATA_ERROR; /* and fall through */ 
            case Z_DATA_ERROR: 
            case Z_MEM_ERROR: 
                inflateEnd(&strm); 
            return ret; 
        }
        
        have = CHUNK - strm.avail_out; 
        totalsize += have; 
        *dest = (char*)realloc(*dest,totalsize); 
        memcpy(*dest + totalsize - have,out,have); 
    } while (strm.avail_out == 0); 
    
    /* clean up and return */ 
    inflateEnd(&strm); 
    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR; 
}

void encodeZip(const string &buffer, string& zipBuf, int& zipLen)
{
    unsigned char _zipSrc[MaxLen];
    unsigned char _zipDst[MaxLen];
    unsigned char  _scBuffer[MaxLen];

    //先对原始内容进行压缩工作
    unsigned int tmpLen = buffer.length();
    memcpy(_zipSrc, buffer.c_str(), tmpLen);
    z_stream c_stream;
    c_stream.zalloc = (alloc_func)0;
    c_stream.zfree = (free_func)0;
    c_stream.opaque = (voidpf)0;
    c_stream.next_in = (Bytef*)_zipSrc;
    c_stream.avail_in = tmpLen;
    c_stream.next_out = (Bytef*)_zipDst;
    c_stream.avail_out = (unsigned int)MaxLen;
    int ret = deflateInit2(&c_stream, Z_DEFAULT_COMPRESSION,Z_DEFLATED, 31,8, Z_DEFAULT_STRATEGY);
    if(ret != Z_OK)
    {
        cout <<"|"<<"deflateInit2 error "<<endl;
        return;
    }
    ret = deflate(&c_stream, Z_FINISH);
    if (ret != Z_STREAM_END)
    {
        deflateEnd(&c_stream);
        cout <<"|"<<"ret != Z_STREAM_END err="<< ret <<endl;
        return;
    }
    
    zipLen = c_stream.total_out;
    ret = deflateEnd(&c_stream);
    if (ret != Z_OK)
    {
        cout <<"|"<<"deflateEnd error "<<endl;
        return;
    }
    //压缩完毕进行返回包组织
    memcpy(_scBuffer, _zipDst, zipLen);
    zipBuf = string((const char *)_scBuffer, zipLen);
    return;
}

int main(void)
{
	std::ifstream f1;
	std::ofstream f2;
	std::string buffer;
	
	////
	char *data = "kjdalkfjdflkjdlkfjdklfjdlkfjlkdjflkdjflddajfkdjfkdfaskf;ldsfk;ldakf;ldskfl;dskf;ld";
	uLong ndata = strlen(data);
	Bytef zdata[BUF_SIZE];
	uLong nzdata = BUF_SIZE;
	Bytef odata[BUF_SIZE];
	uLong nodata = BUF_SIZE;
	////
	
	f1.open("dump1");
	
	if(!f1)
	{
		std::cout <<"open test.gz file failed"<<std::endl;
		return 0;
	}
	
	char c[80];
	char buf[30000];
	memset(buf, 0, sizeof(buf));
	int len = 0;
	while(!f1.eof())
	{
		f1.read(c,80);
	
		//std::string str(c);
		//buffer += str;
		
		//std::cout<<"len="<<len<<" bufsize="<<buffer.size()<<std::endl;
		memcpy(buf+len, c, f1.gcount());
		len += f1.gcount();
	}
	fprintf(stdout, "len = %d\n", len);
	f1.close();
	
	nodata = BUF_SIZE;
	buffer = std::string(buf);
	std::string zipBuf;
	int zipLen;
	encodeZip(buffer, zipBuf, zipLen);
	
	f2.open("dump2.gz");
	//f2.write(zipBuf, zipLen);
	f2<<zipBuf;
	f2.close();
	//if (0 == httpgzdecompress((Byte *)buf, len, odata,  &nodata))
	//{
	//	fprintf(stdout, "%d %s\n", nodata, odata);
	//}
	//else
	//{
	//	fprintf(stdout, "Fuck error\n");
	//}
}
