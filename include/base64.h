#ifndef _BASE_64_
#define _BASE_64_

char *base64_encode(const char *data, int data_len);
char *base64_decode(const char *data, int data_len);

#endif
