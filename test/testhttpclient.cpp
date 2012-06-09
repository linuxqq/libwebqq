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
#include <vector>
#include <algorithm>
#include "QQAuthentication.h"
#include "json/json.h"
#include "QQDebug.h"
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
    std::string usr="1421032531";//"1421032531";
    std::string password="1234567890";
    std::string uri("http://check.ptlogin2.qq.com/check?uin="+usr+"&appid=1003903&r=0.09714792561565866");
    HttpClient * request = Singleton<HttpClient>::getInstance();
    std::string result = request->requestServer(uri);
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
    EncodePass encoder = EncodePass(password, vcode, uin);
    uri = "http://ptlogin2.qq.com/login?u="+usr+"&p="+encoder.encode()+"&verifycode="+ vcode +"&webqq_type=10&remember_uin=1&login2qq=1&aid=1003903&u1=http%3A%2F%2Fweb.qq.com%2Floginproxy.html%3Flogin2qq%3D1%26webqq_type%3D10&h=1&ptredirect=0&ptlang=2052&from_ui=1&pttype=1&dumy=&fp=loginerroralert&action=1-16-9209&mibao_css=m_webqq&t=1&g=1";

    result = Singleton<HttpClient>::getInstance()->requestServer(uri);
    std::string ptvfsession;
    std::string ptwebqq;
    if (Singleton<HttpClient>::getInstance()->getValueFromCookie("ptwebqq", ptwebqq) == true)
    {
        std::cout<<ptwebqq<<endl;
    }
    else
        cout<<"Fuck fail"<<endl;

    std::cout<<result<<endl;

    uri = "http://d.web2.qq.com/channel/login2";
    body = "r=%7B%22status%22%3A%22online%22%2C%22ptwebqq%22%3A%22"+ ptwebqq +  "%22%2C%22passwd_sig%22%3A%22%22%2C%22clientid%22%3A%2298403775%22%2C%22psessionid%22%3Anull%7D&clientid=98403775&psessionid=null";
    request = Singleton<HttpClient>::getInstance();
    std::list<std::string> headers;
    headers.push_back("Referer: http://d.web2.qq.com/proxy.html?v=20110331002&callback=2");
    std::vector<curlpp::OptionBase*> settings;
    settings.push_back(new curlpp::options::HttpHeader(headers));
    request->setOptions(settings);
    result = request->requestServer(uri, body);
    std::cout<<result<<std::endl;

     std::string vfwebqq;

    Json::Reader jsonReader;
    Json::Value root, item;
    jsonReader.parse(result, root, false);

    item = root["result"]["psessionid"];
    std::cout<<item<<std::endl;
    vfwebqq= root["result"]["vfwebqq"].toStyledString();
    pos_1 = vfwebqq.find("\"");
    pos_2 = vfwebqq.find_last_of("\"");
    vfwebqq = vfwebqq.substr(pos_1 +2, pos_2-pos_1-3);
    uri="http://s.web2.qq.com/api/get_user_friends2";
    body = "r=%7B%22h%22%3A%22hello%22%2C%22vfwebqq%22%3A%22"+ vfwebqq +"%22%7D";
    headers.clear();
    headers.push_back("Referer: http://s.web2.qq.com/proxy.html?v=20101025002");
    std::vector<curlpp::OptionBase*> settings2;
    settings2.push_back(new curlpp::options::HttpHeader(headers));
    request = Singleton<HttpClient>::getInstance();
    request->setOptions(settings2);
    result = request->requestServer(uri, body);
    std::cout<<result<<std::endl;
    //jsonReader.parse(result, root, false);
    //item = root["result"];
    //std::cout<<item<<std::endl;

    uri="http://s.web2.qq.com/api/get_group_name_list_mask2";
    body="r=%7B%22vfwebqq%22%3A%22"+vfwebqq+"%22%7D";
    debug_info("%s",vfwebqq.c_str());
    request = Singleton<HttpClient>::getInstance();
    headers.clear();
    headers.push_back("Referer: http://s.web2.qq.com/proxy.html?v=20110412001");
    std::vector<curlpp::OptionBase*> settings3;
    settings3.push_back(new curlpp::options::HttpHeader(headers));
    request->setOptions(settings3);
    result = request->requestServer(uri, body);
    cout<<result<<endl;

    jsonReader.parse(result, root, false);
    item = root["result"];
    std::cout<<item<<std::endl;
    return 0;
}
