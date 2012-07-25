/**
 * @file   testmd5.c
 * @author Xiang Wang <xiang_wang@trendmicro.com.cn>
 * @date   Wed May 16 16:01:20 2012
 *
 * @brief
 *
 *
 */

#include "QQMD5.h"
#include <stdio.h>
#define debug_info printf
#include <string>
#include <iostream>
#include "QQAuthentication.h"
int main()
{
    /*A0B0234C253E224C973A76F47ED05406*/
    EncodePass encodepass("1234567890", "!B55", "\\x00\\x00\\x00\\x00\\x54\\xb3\\x3c\\x53");
    std::string result = encodepass.encode();
    std::cout<<"Result:"<<result<<std::endl;
    return 0;
}

