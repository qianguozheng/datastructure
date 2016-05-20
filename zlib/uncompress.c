
#include <zlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

using namespace std;

class CUncompress
{
	
public:
	CUncompress(string & sUncompressData);
	~CUncompress();
public:
	int Init(const char * pszCompressed, int nCompressedLen, bool bIsGzip=false);
	int UncompressData();
	char * GetErrMsg()
	{
		return m_szErrMsg;
	}
private:
	string & m_sData;
	char * m_pszDataChunk;
	z_stream m_objStream;
	char m_szErrMsg[1024];
	bool m_bIsGzip;
	
};

//-----------CPP

static const int DSIZE = 0x10000;

CUncompress::CUncompress(string & sUncompressData):m_sData(sUncompressData)
{
	memset(m_szErrMsg, 0, 1024);
	m_pszDataChunk = new char[DSIZE];
	m_bIsGzip = false;
}

CUncompress::~CUncompress()
{
	if (m_pszDataChunk != NULL)
	{
		delete [] m_pszDataChunk;
	}
}

int CUncompress::Init(const char * pszCompressed, int nCompressedLen, bool bIsGzip)
{
	int nErrCode = 0;
	m_objStream.zalloc = (alloc_func)Z_NULL;
	m_objStream.zfree = (free_func)Z_NULL;
	m_objStream.opaque = 0;
	m_objStream.next_in = NULL;
	m_objStream.avail_in = 0;
	if (bIsGzip)
	{
		m_bIsGzip = true;
		if (strcmp(zlibVersion(), "1.2.0.4") < 0)
		{
			snprintf(m_szErrMsg, 1024, "zlib version < 1.2.0.4");
			return -1;
		}
		//if ((nErrCode = inflateInit2(&m_objStream, MAX_WBITS + 32)) != Z_OK)
		if ((nErrCode = inflateInit2(&m_objStream, 31)) != Z_OK)
		{
			snprintf(m_szErrMsg, 1024, "inflateInit2 failed, err code=%d", nErrCode);
			return -2;
		}
	}
	else
	{
		if ((nErrCode = inflateInit(&m_objStream)) != Z_OK)
		{
			snprintf(m_szErrMsg, 1024, "inflateInit failed, err code=%d", nErrCode);
			return -3;
		}
	}
	m_objStream.next_in = (Bytef*)pszCompressed;
	m_objStream.avail_in = (uInt)nCompressedLen;
	return 0;
}

int CUncompress::UncompressData()
{
	bool bRetry = true;
	uInt unRead = m_objStream.avail_in;
	Bytef * pszInData = m_objStream.next_in;
	int nStatus = 0;
	for (;;)
	{
		m_objStream.next_out = (Bytef*)m_pszDataChunk;
		m_objStream.avail_out = DSIZE;
		nStatus = inflate(&m_objStream, Z_SYNC_FLUSH);
		if (nStatus == Z_OK || nStatus == Z_STREAM_END)
		{
			bRetry = false;
			if (DSIZE - m_objStream.avail_out)
			{
				m_sData.append(m_pszDataChunk, DSIZE - m_objStream.avail_out);
			}
			if (nStatus == Z_STREAM_END || (nStatus == Z_OK && m_objStream.avail_in == 0))
			{
				if ((nStatus = inflateEnd(&m_objStream)) == Z_OK)
				{
					return 0;
				}
				else
				{
					snprintf(m_szErrMsg, 1024, "inflateEnd failed, err code=%d", nStatus);
					return -1;
				}
			}
		}
		else if ((!m_bIsGzip) && bRetry && nStatus == Z_DATA_ERROR)
		{
			//if has no zlib header, try another way
			(void)inflateEnd(&m_objStream);
			if ((nStatus = inflateInit2(&m_objStream, -MAX_WBITS)) != Z_OK)
			{
				snprintf(m_szErrMsg, 1024, "data has no zlib header, try another way inflateInit2 failed, err cod=%d", nStatus);
				return -2;
			}
			m_objStream.next_in = pszInData;
			m_objStream.avail_in = unRead;
			bRetry = false;
			continue;
		}
		else
		{
			(void)inflateEnd(&m_objStream);
			snprintf(m_szErrMsg, 1024, "error when inflate compressed, err code=%d", nStatus);
			return -3;
		}
	}
	return 0;
}



/* Uncompress gzip data */
/* zdata 数据 nzdata 原数据长度 data 解压后数据 ndata 解压后长度 */
int gzdecompress(Byte *zdata, uLong nzdata,
                 Byte *data, uLong *ndata)
{
    int err = 0;
    z_stream d_stream = {0}; /* decompression stream */
    static char dummy_head[2] = {
        0x8 + 0x7 * 0x10,
        (((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
    };
    d_stream.zalloc = NULL;
    d_stream.zfree = NULL;
    d_stream.opaque = NULL;
    d_stream.next_in  = zdata;
    d_stream.avail_in = 0;
    d_stream.next_out = data;
    //只有设置为MAX_WBITS + 16才能在解压带header和trailer的文本
    if(inflateInit2(&d_stream, MAX_WBITS + 16) != Z_OK) return -1;
    //if(inflateInit2(&d_stream, 47) != Z_OK) return -1;
    while(d_stream.total_out < *ndata && d_stream.total_in < nzdata) {
        d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
        if((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END) break;
        if(err != Z_OK) {
            if(err == Z_DATA_ERROR) {
                d_stream.next_in = (Bytef*) dummy_head;
                d_stream.avail_in = sizeof(dummy_head);
                if((err = inflate(&d_stream, Z_NO_FLUSH)) != Z_OK) {
					std::cout<<"Fuck"<<std::endl;
                    return -1;
                }
            } else{
				std::cout<<"Fuckxxx"<<std::endl;
				 return -1;
			 }
        }
    }
    if(inflateEnd(&d_stream) != Z_OK) return -1;
    *ndata = d_stream.total_out;
    return 0;
}

char *decompress(char *compressed, int wsize, int input_len, int bufsize)
{
	printf("wsize=%d, input_len=%d, bufsize=%d\n", wsize, input_len, bufsize);
    Byte *result_str;
    Byte *input = (Byte *)compressed;
    int length, err;
    //int wsize=DEF_WBITS;
    //Py_ssize_t r_strlen=DEFAULTALLOC;
    int r_strlen = bufsize;
    length = input_len;
    z_stream zst;

    //if (!PyArg_ParseTuple(args, "s#|in:decompress",
    //                      &input, &length, &wsize, &r_strlen))
    //    return NULL;

    if (r_strlen <= 0)
        r_strlen = 1;

    zst.avail_in = length;
    zst.avail_out = r_strlen;

    //if (!(result_str = PyString_FromStringAndSize(NULL, r_strlen)))
    //    return NULL;
    result_str = (Byte *)malloc(r_strlen);
	memset(result_str, 0, r_strlen)
	;
    zst.zalloc = (alloc_func)NULL;
    zst.zfree = (free_func)Z_NULL;
    zst.next_out = (Byte *)(result_str);
    zst.next_in = (Byte *)input;
    err = inflateInit2(&zst, wsize);

    switch(err) {
    case(Z_OK):
        break;
    case(Z_MEM_ERROR):
        //PyErr_SetString(PyExc_MemoryError,
        printf("Out of memory while decompressing data\n");
        goto error;
    default:
        inflateEnd(&zst);
        //zlib_error(zst, err, "while preparing to decompress data");
        printf("while preparing to decompress data\n");
        goto error;
    }

    do {
        //Py_BEGIN_ALLOW_THREADS
        //err=inflate(&zst, Z_FINISH);
        
        err=inflate(&zst, Z_SYNC_FLUSH);
        //Py_END_ALLOW_THREADS
		printf(" inflate: err=%d\n", err);
        switch(err) {
        case(Z_STREAM_END):
            break;
        case(Z_BUF_ERROR):
            /*
             * If there is at least 1 byte of room according to zst.avail_out
             * and we get this error, assume that it means zlib cannot
             * process the inflate call() due to an error in the data.
             */
            if (zst.avail_out > 0) {
                //zlib_error(zst, err, "while decompressing data");
                printf("while decompressing data\n");
                inflateEnd(&zst);
                goto error;
            }
            /* fall through */
        case(Z_OK):
            /* need more memory */
            /*if (_PyString_Resize(&result_str, r_strlen << 1) < 0) {
                inflateEnd(&zst);
                goto error;
            }*/
            //zst.next_out = (unsigned char *)PyString_AS_STRING(result_str)
            zst.next_out = (unsigned char *)(result_str) \
                + r_strlen;
            zst.avail_out = r_strlen;
            r_strlen = r_strlen << 1;
            break;
        default:
            inflateEnd(&zst);
            //zlib_error(zst, err, "while decompressing data");
            printf("%d: while decompressing data, %d\n", __LINE__, err);
            goto error;
        }
    } while (err != Z_STREAM_END);

    err = inflateEnd(&zst);
    if (err != Z_OK) {
        //zlib_error(zst, err, "while finishing data decompression");
        printf("while finishing data decompression\n");
        goto error;
    }

    //_PyString_Resize(&result_str, zst.total_out);
    //global_decompress_err = err;
    return (char *)result_str;

 error:
    //Py_XDECREF(result_str);
    if (result_str)
    {
		free(result_str);
	}
    return NULL;
}

int main(int argc, char *argv[])
{
	
	std::ifstream f1;
	std::string buffer;
	
	f1.open("test.gz");
	
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
		
		std::cout<<"len="<<len<<" bufsize="<<buffer.size()<<std::endl;
		memcpy(buf+len, c, f1.gcount());
		len += f1.gcount();
	}
	f1.close();

	//std::cout<<"buffer size="<<buffer.size()<<std::endl;
	
#if 0
	std::string uncompress;
	CUncompress dcp(uncompress);
	//CUncompress::Init(char * pszCompressed, int nCompressedLen, bool bIsGzip)
	
	dcp.Init((const char *)buf, len, true);
	dcp.UncompressData();
	std::cout<<"Error:"<<dcp.GetErrMsg()<<std::endl;
	
	std::cout<<"Uncompressed Data="<<uncompress<<std::endl;
	
	char rawdata[1200000];
	int rawlen = 1200000;
	gzdecompress((Byte *)buf, len, (Byte *)rawdata, (long unsigned int*) &rawlen);
	std::cout<<"raw data="<<rawdata<<std::endl;
#else

	// char *decompress(char *compressed, int wsize, int input_len, int bufsize)
	char *result = NULL;
	
	result = decompress(buf, MAX_WBITS+16, len, 1200000);
	printf("result=%s", result);
	
#endif
	
}
