#ifndef MD5_H
#define MD5_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>

#define MD5_HASHBYTES 16

typedef struct MD5Context {
    u_int32_t buf[4];
    u_int32_t bits[2];
    unsigned char in[64];
} MD5_CTX;

char* lutil_md5_file(const char *filename, char *buf);
unsigned char * lutil_md5_digest(const unsigned char * data,
                                 unsigned int len , unsigned char *buf);
char* lutil_md5_data(const unsigned char *data,
                     unsigned int len,  char *buf);

#endif
