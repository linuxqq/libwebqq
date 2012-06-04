/**
 * @file   testmd5.c
 * @author Xiang Wang <xiang_wang@trendmicro.com.cn>
 * @date   Wed May 16 16:01:20 2012
 * 
 * @brief  
 * 
 * 
 */

#include "md5.h"
#include <stdio.h>
#define debug_info printf
#include <string.h>
int main()
{
    unsigned char buf[128];
    unsigned char uin[] = "\\x00\\x00\\x00\\x00\\x54\\xb3\\x3c\\x53"; 
    unsigned char password[] = "1234567890" ;/*E572FFB46966601CCACC432C6A4F4F80
    */
    unsigned char verifycode[] ="!RUN";
    memset(buf, 0 , 128);
    lutil_md5_digest(password,strlen(password), (char *)buf);
    debug_info("%s\n", buf);
    strncat((char*) buf, uin, strlen(uin));
    debug_info("%s\n", buf);
    lutil_md5_data(buf, strlen(buf), (const unsigned char *)buf);
    debug_info("%s\n", buf) ;
    lutil_md5_data(buf, 16,(const unsigned char *)buf);
    debug_info("%s\n", buf);
    
    int i = 0;
    
    for( i = 0 ;i < strlen(buf); i++)
    {
        if (islower(buf[i]))
            buf[i]= toupper(buf[i]);
    }
    strncat(buf, verifycode, strlen(verifycode)+1);
    debug_info("%s\n", buf);
    
    lutil_md5_data(buf, strlen(buf), (const unsigned char *)buf);
    
    for( i = 0 ;i < strlen(buf); i++)
    {
        if (islower(buf[i]))
            buf[i]= toupper(buf[i]);
    }
    debug_info("%s\n", buf);
}

