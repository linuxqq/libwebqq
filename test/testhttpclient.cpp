/**
 * @file   testhttpclient.cpp
 * @author Xiang Wang <xiang_wang@trendmicro.com.cn>
 * @date   Wed May 30 19:33:37 2012
 *
 * @brief
 *
 *
 */

#include <HttpClient.h>
#include <iostream>
#include <string>
int main()
{
    std::string uri("http://check.ptlogin2.qq.com/check?uin=1421032531&appid=1003903&r=0.09714792561565866");
    HttpClient *httpclient = Singleton<HttpClient>::getInstance();
    std::string result = httpclient->requestServer(uri);
    std::cout<<result<<std::endl;

    HttpClient *httpclient2 = Singleton<HttpClient>::getInstance();
    result = httpclient2->requestServer(uri);
    std::cout<<result<<std::endl;

    delete httpclient;
}
