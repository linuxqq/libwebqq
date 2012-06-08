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
#include <algorithm>
#include "QQAuthentication.h"
using namespace std;
void Tokenize(const string& str,
                      vector<string>& tokens,
                      const string& delimiters = " ")
{
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}
int main()
{
    std::string uri("http://check.ptlogin2.qq.com/check?uin=1421032531&appid=1003903&r=0.09714792561565866");
    std::string result = Singleton<HttpClient>::getInstance()->requestServer(uri);
    std::cout<<result<<std::endl;
    size_t pos_1 = result.find("(");
    size_t pos_2 = result.find(")");
    std::string body = result.substr(pos_1 +2, pos_2-pos_1-3);
    std::cout<<body<<std::endl;
    std::vector<std::string > vec;

    Tokenize(body, vec, "\',\'");
    std::string vcode = vec[1];
    std::string uin= vec[2];


    result = Singleton<HttpClient>::getInstance()->requestServer("http://ui.ptlogin2.qq.com/cgi-bin/ver");
    EncodePass encoder = EncodePass("1234567890", vcode, uin);
    uri = "http://ptlogin2.qq.com/login?u=1421032531&p="+encoder.encode()+"&verifycode="+ vcode +"&webqq_type=10&remember_uin=1&login2qq=1&aid=1003903&u1=http%3A%2F%2Fweb.qq.com%2Floginproxy.html%3Flogin2qq%3D1%26webqq_type%3D10&h=1&ptredirect=0&ptlang=2052&from_ui=1&pttype=1&dumy=&fp=loginerroralert&action=1-16-9209&mibao_css=m_webqq&t=1&g=1";

    result = Singleton<HttpClient>::getInstance()->requestServer(uri);
    std::string ptvfsession;
    if (Singleton<HttpClient>::getInstance()->getValueFromCookie("ptvfsession", ptvfsession) == true)
    {
        std::cout<<ptvfsession<<endl;
    }

    else
        cout<<"Fuck fail"<<endl;
    cout<<result<<endl;


}
