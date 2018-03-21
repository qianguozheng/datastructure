#include <stdio.h>
#include <stdlib.h>

/* type->string mapping: this is also used by the name-hash function as a mixing table. */
static const struct {
  unsigned int type;
  const char * const name;
} typestr[] = {
  { 1,   "A" },
  { 2,   "NS" },
  { 5,   "CNAME" },
  { 6,   "SOA" },
  { 10,  "NULL" },
  { 11,  "WKS" },
  { 12,  "PTR" },
  { 13,  "HINFO" },	
  { 15,  "MX" },
  { 16,  "TXT" },
  { 22,  "NSAP" },
  { 23,  "NSAP_PTR" },
  { 24,  "SIG" },
  { 25,  "KEY" },
  { 28,  "AAAA" },
  { 33,  "SRV" },
  { 35,  "NAPTR" },
  { 36,  "KX" },
  { 37,  "CERT" },
  { 38,  "A6" },
  { 39,  "DNAME" },
  { 41,  "OPT" },
  { 43,  "DS" },
  { 46,  "RRSIG" },
  { 47,  "NSEC" },
  { 48,  "DNSKEY" },
  { 50,  "NSEC3" },
  { 249, "TKEY" },
  { 250, "TSIG" },
  { 251, "IXFR" },
  { 252, "AXFR" },
  { 253, "MAILB" },
  { 254, "MAILA" },
  { 255, "ANY" }
};

int hash_size = 1024;
void hash_bucket(char *name)
{
	unsigned int c, val = 017465; /* Barker code - minimum self-correlation in cyclic shift */
	const unsigned char *mix_tab = (const unsigned char*)typestr; 

	while((c = (unsigned char) *name++))
	{
		/* don't use tolower and friends here - they may be messed up by LOCALE */
		if (c >= 'A' && c <= 'Z')
			c += 'a' - 'A';
		val = ((val << 7) | (val >> (32 - 7))) + (mix_tab[(val + c) & 0x3F] ^ c);
    } 
  
  /* hash_size is a power of two */
	printf("bucket[%s]=[%llu]\n",name, ((val ^ (val >> 16)) & (hash_size - 1)));
	
}

int main(){
	hash_bucket("www.baidu.com");
	hash_bucket("inews.gtimg.com");
	hash_bucket("micloud.xiaomi.net");
	hash_bucket("pnewsapp.tc.qq.com");
	hash_bucket("o.data.mistat.xiaomi.com");
	hash_bucket("storeconfig.mistat.xiaomi.com");
}
