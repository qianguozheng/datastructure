#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

//https://github.com/qianguozheng/datastructure.git
//using namespace std;
#define MOD_GZIP_ZLIB_WINDOWSIZE 15
std::string decompress_gzip(const std::string& str)
{
    z_stream zst;                        // z_stream is zlib's control structure
    memset(&zst, 0, sizeof(zst));
    if (inflateInit2(&zst, MOD_GZIP_ZLIB_WINDOWSIZE + 16) != Z_OK)
    {//debugs(98,1, "error failed while decompressing.");
            std::cout << "inflateInit2 failed"<<std::endl;
	}
    zst.next_in = (Bytef*)str.data();
    zst.avail_in = str.size();
    int ret;
    char outbuffer[327680];
    std::string outstring;

	std::cout << "avail_in="<<zst.avail_in<<std::endl;
    // get the decompressed bytes blockwise using repeated calls to inflate
    do {
        zst.next_out = reinterpret_cast<Bytef*>(outbuffer);
        zst.avail_out = sizeof(outbuffer);
        ret = inflate(&zst, 0);

        if (outstring.size() < zst.total_out) {
            outstring.append(outbuffer, zst.total_out - outstring.size());
        }
		std::cout << "Z_OK=" << ret <<std::endl;
    } while (ret == Z_OK);

    inflateEnd(&zst);

    if (ret != Z_STREAM_END) {          // an error occurred that was not EOF
        //std::ostringstream oss;
                //debugs(98,2, oss << "Exception during zlib decompression: (" << ret << ") " << zst.msg);
                std::cout << "Z_STREAM_END failed by Richard"<<std::endl;
    }

    return outstring;
} 

#if 0
static const int TheWindowBits = 15+16;
int UncompressData( const Byte* abSrc, int nLenSrc, Byte* abDst, int nLenDst )
{

    z_stream zInfo ={0};
    zInfo.total_in= zInfo.avail_in= nLenSrc;
    zInfo.total_out= zInfo.avail_out= nLenDst;
    zInfo.next_in= (Byte*)abSrc;
    zInfo.next_out= abDst;

    int nErr, nRet= -1;
    //nErr= inflateInit( &zInfo ); // zlib function
    Debug(flApplication|ilNormal) <<"zInfo.total_in"<<zInfo.total_in;
    Debug(flApplication|ilNormal) <<"zInfo.total_out"<<zInfo.total_out;
    nErr = inflateInit2(&zInfo, TheWindowBits);
    Debug(flApplication|ilNormal) <<"inflateInit2 nErr="<<nErr;
    if ( nErr == Z_OK ) {
        nErr= inflate( &zInfo, Z_NO_FLUSH ); // zlib function
        if ( nErr == Z_STREAM_END ) {
            nRet= zInfo.total_out;
        }
    }
    else
    {
		Debug(flApplication|ilNormal) <<"inflateInit2 nErr=ff"<<nErr;
		if (nErr == Z_DATA_ERROR)
		{
			Debug(flApplication|ilNormal) <<"Z_DATA_ERR failed by Richard";
		}
	}
	Debug(flApplication|ilNormal) <<"Before inflateEnd"<<nErr;
    inflateEnd( &zInfo ); // zlib function
    return( nRet ); // -1 or len of output
}

#endif

/* Compress from file source to file dest until EOF on source.
   def() returns Z_OK on success, Z_MEM_ERROR if memory could not be
   allocated for processing, Z_STREAM_ERROR if an invalid compression
   level is supplied, Z_VERSION_ERROR if the version of zlib.h and the
   version of the library linked do not match, or Z_ERRNO if there is
   an error reading or writing the files. */
int def(FILE *source, FILE *dest, int level)
{
    int ret, flush;
    unsigned have;
    z_stream strm;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    /* allocate deflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit(&strm, level);
    if (ret != Z_OK)
        return ret;

    /* compress until end of file */
    do {
        strm.avail_in = fread(in, 1, CHUNK, source);
        if (ferror(source)) {
            (void)deflateEnd(&strm);
            return Z_ERRNO;
        }
        flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = in;

        /* run deflate() on input until output buffer not full, finish
           compression if all of source has been read in */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = deflate(&strm, flush);    /* no bad return value */
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            have = CHUNK - strm.avail_out;
            if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
                (void)deflateEnd(&strm);
                return Z_ERRNO;
            }
        } while (strm.avail_out == 0);
        assert(strm.avail_in == 0);     /* all input will be used */

        /* done when last data in file processed */
    } while (flush != Z_FINISH);
    assert(ret == Z_STREAM_END);        /* stream will be complete */

    /* clean up and return */
    (void)deflateEnd(&strm);
    return Z_OK;
}
int main(int argc, char *argv[])
{
	//std::string buffer;
	std::string decoded;

#if 1
	std::ifstream f1;
	
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
		
		//std::cout<<"len="<<len<<" bufsize="<<buffer.size()<<std::endl;
		memcpy(buf+len, c, f1.gcount());
		len += f1.gcount();
	}
	fprintf(stdout, "len = %d\n", len);
	f1.close();
	std::string buffer(buf);
	std::cout<<"buffer size="<<buffer.size()<<std::endl;
#else
	FILE *fp = NULL;
	fp = fopen("test.gz", "r");
	if (!fp)
	{
		std::cout<<"Open file failed"<<std::endl;
		return 0;
	}
	
	char *buf = (char *)malloc(30000);
	char tmp[1025];
	//memset(buf, 0, );
	memset(tmp, 0, sizeof(tmp));
	int rc = 0;
	while( (rc = fread(tmp,sizeof(unsigned char), 1024,fp)) != 0 )
	{
		memcpy(buf, tmp, 1024);
		std::cout<<"read once"<<std::endl;
		memset(tmp, 0, sizeof(tmp));
	}
	
#endif
	//std::string tmp1(buf);
	//buffer = tmp1;
	std::cout<<"buffer size="<<buffer.size()<<std::endl;
	decoded = decompress_gzip(buffer);
	//UncompressData( const Byte* abSrc, int nLenSrc, Byte* abDst, int nLenDst )
	//UncompressData((const Byte*)buf, , , );
	std::cout<<decoded;
	
//	free(buf);
	
	return 0;
}

