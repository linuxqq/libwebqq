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
#include "md5.h"
#include "QQDebug.h"
#include <cctype>
#include <algorithm>
#include <cstdlib>
#include <cstring>
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

std::string EncodePass::hexchar2bin(const std::string data)
{
    char*buf = new char[ data.size()/2 +1];
    int j = 0;
    for (int i = 0; i < data.size(); i = i + 2) {
        char ints[3]= {0};
        memset(ints, 0, sizeof(ints));
        memcpy( ints, data.c_str() + i , 2);
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
    std::string result = std::string(buf);
    delete buf;
    return result;
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

    std::string result = hexchar2bin( md5(passcode)) + uin_str;
    result = md5(md5(result) + vcode);
    return result;
}
