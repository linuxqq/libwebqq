/**
 * @file   QQAuthentication.cpp
 * @author Xiang Wang <xiang_wang@trendmicro.com.cn>
 * @date   Thu Jun  7 13:55:01 2012
 *
 * @brief
 *
 *
 */

#include "QQAuthentication.h"
#include <iostream>
#include <string>
#include "QQMD5.h"
#include "QQDebug.h"
#include <cctype>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <vector>

EncodePass::EncodePass(const std::string password, const std::string vcode, const std::string uin)
{
    this->passcode  = password;
    this->vcode = vcode;
    this->uin = uin;
    std::transform(this->vcode.begin(),this-> vcode.end(), this->vcode.begin(), ::toupper);

}

std::string EncodePass::md5(const std::string data)
{
    unsigned char * p = NULL;
    const unsigned char * ptr = reinterpret_cast<const unsigned char *>(data.c_str());
    p = (unsigned char *)lutil_md5_data(ptr , data.size(),  (char *)(p) );
    if ( p == NULL)
    {
        debug_error("EncodePass Error");
    }

    std::string result = std::string( reinterpret_cast<char *>(p));
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);

    if (p ) free(p);
    return result;
}

std::string EncodePass::md5(const unsigned char *data, int length)
{
    unsigned char * p = NULL;
    p = (unsigned char *)lutil_md5_data(data , length,  (char *)(p) );
    if ( p == NULL)
    {
        debug_error("EncodePass Error");
    }

    std::string result = std::string( reinterpret_cast<char *>(p));
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);

    if (p ) free(p);
    return result;
}

char *EncodePass::hexchar2bin(const std::string data, int &length)
{
    length = data.size()/2;
    char*buf = new char[ data.size()/2];
    int j = 0;
    const char *data_str;
    data_str = data.c_str();
    memset(buf, 0, length);
    for (int i = 0; i < data.size(); i = i + 2) {
        char ints[3]= {0};
        memset(ints, 0, sizeof(ints));
        memcpy(ints, data_str + i , 2);
        char *endptr;
        long int iv = strtol(ints,&endptr,16 );
        if  ( ints == endptr)
        {
            debug_error("No string to parse.");
            break;
        }
        else if ( *endptr != '\0' )
        {
            debug_error("Further characters after number: %s", endptr);
            break;
        }
        else
            buf[j++] =( unsigned char ) iv;
    }
    return buf;
}

std::string EncodePass::encode()
{

    char * str = strdup(uin.c_str());
    char* tok = strtok ( str, "\\x");

    std::vector<std::string> parts;
    while ( tok != NULL ) {
        parts.push_back( tok );
        tok = strtok (NULL, "\\x");
    }

    std::string uin_str;
    for ( std::vector<std::string>::iterator it = parts.begin();
          it != parts.end(); it ++)
    {
        char ints[3]= {0};
        memset(ints, 0, sizeof(ints));
        memcpy( ints,  it->c_str() , 2);
        char *endptr;
        long int iv = strtol(ints,&endptr,16 );
        if  ( ints == endptr)
        {
            debug_error("No string to parse.");
            break;
        }
        else if ( *endptr != '\0' )
        {
            debug_error("Further characters after number: %s", endptr);
            break;
        }
        else
            uin_str +=  ( unsigned char ) iv;
    }

    delete [] str;
    std::string result;
    int len1, len2;
    char *tmp1 = hexchar2bin(md5(passcode), len1);
    len2 = len1+uin_str.size();
    char *tmp2 = new char[len2];
    memcpy(tmp2, tmp1, len1);
    memcpy(tmp2+len1, uin_str.c_str(), uin_str.size());
    delete []tmp1;
    result = md5(md5((unsigned char *)tmp2, len2) + vcode);
    delete []tmp2;
    return result;
}
