/**
 * @file   QQAuthentication.h
 * @author Xiang Wang <xiang_wang@trendmicro.com.cn>
 * @date   Thu Jun  7 13:54:41 2012
 *
 * @brief
 *
 *
 */

#ifndef __QQ_AUTHENTICATION_H__
#define __QQ_AUTHENTICATION_H__

#include <string>

class EncodePass{

    std::string passcode;
    std::string vcode;
    std::string uin;

public:
    explicit EncodePass(const std::string passcode,const  std::string vcode,const  std::string uin);
    std::string encode();
    ~EncodePass(){ }
private:

    std::string  hexchar2bin(const std::string data);
    std::string  md5(const std::string data);

};



#endif
