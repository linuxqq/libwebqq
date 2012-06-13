/**
 * @file   QQPlugin.cpp
 * @author Xiang Wang <xiang_wang@trendmicro.com.cn>
 * @date   Wed Jun 13 11:02:04 2012
 *
 * @brief
 *
 *
 */

#include "QQPlugin.h"
#include "QQUtil.h"
#include "QQAuthentication.h"
#include <iostream>
#include "ThreadPool.h"
#include "HttpClient.h"
#include "QQDebug.h"
#include "json/json.h"
#include "QQTypes.h"
#include "json/writer.h"
using namespace QQUtil;

ResourceManager::ResourceManager()
{
    rw_mutex = ( pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
}

ResourceManager::~ResourceManager()
{

}

void ResourceManager::lock()
{
    pthread_mutex_lock(&rw_mutex);
}

void ResourceManager::ulock()
{
    pthread_mutex_unlock(&rw_mutex);
}


QQPlugin::QQPlugin()
{
    ThreadPool::init(4);
    res = Singleton<ResourceManager>::getInstance();

    clientid="98403775";
}

QQPlugin::~QQPlugin()
{
    delete res;
    ThreadPool::sync_all();
    ThreadPool::done();
}

bool QQPlugin::webqq_login(const std::string & user, const std::string & password, const std::string & status)
{

    std::string uri("http://check.ptlogin2.qq.com/check?uin="+\
                    user+"&appid=1003903&r=0.09714792561565866");

    HttpClient * request = Singleton<HttpClient>::getInstance();
    std::string result = request->requestServer(uri);

    size_t pos_1 = result.find("(");
    size_t pos_2 = result.find(")");

    std::string body = result.substr(pos_1
                                     +2, pos_2-pos_1-3);

    std::vector<std::string > vec;

    Tokenize(body, vec, "\',\'");
    std::string vcode = vec[1];
    std::string uin= vec[2];

    result = Singleton<HttpClient>::getInstance()->requestServer("http://ui.ptlogin2.qq.com/cgi-bin/ver");

    EncodePass encoder = EncodePass(password, vcode, uin);

    uri = "http://ptlogin2.qq.com/login?u="+user+ \
          "&p="+encoder.encode()+"&verifycode="+\
          vcode + \
          "&webqq_type=10&remember_uin=1&login2qq=1&aid=1003903"
          "&u1=http%3A%2F%2Fweb.qq.com%2Floginproxy.html%3Flogin2qq%3D1%26webqq_type%3D10"
          "&h=1&ptredirect=0&ptlang=2052&from_ui=1"
          "&pttype=1&dumy=&fp=loginerroralert&action=1-16-9209&mibao_css=m_webqq&t=1&g=1";

    result = Singleton<HttpClient>::getInstance()->requestServer(uri);

    if (Singleton<HttpClient>::getInstance()->getValueFromCookie("ptwebqq", ptwebqq) == false)
    {
        debug_error("Can not get ptwebqq from Cookies ... (%s,%d)", \
                    __FILE__ ,  __LINE__);
        return false;
    }

    uri = "http://d.web2.qq.com/channel/login2";
    body = "r=%7B%22status%22%3A%22"+status +\
           "%22%2C%22ptwebqq%22%3A%22"+ ptwebqq +\
           "%22%2C%22passwd_sig%22%3A%22%22%2C%22clientid%22%3A%22"+\
           clientid+"%22%2C%22psessionid%22%3Anull%7D&clientid="+\
           clientid+"&psessionid=null";

    request = Singleton<HttpClient>::getInstance();

    std::list<std::string> headers;

    headers.push_back("Referer: http://d.web2.qq.com/proxy.html?v=20110331002&callback=2");
    std::vector<curlpp::OptionBase*> settings;

    settings.push_back(new curlpp::options::HttpHeader(headers));

    request->setOptions(settings);
    result = request->requestServer(uri, body);

    try
    {
        Json::Reader jsonReader;
        Json::Value root;
        jsonReader.parse(result, root, false);
        int retcode = root["retcode"].asInt();
        psessionid = root["result"]["psessionid"].asString();
        vfwebqq = root["result"]["vfwebqq"].asString();

        if ( 0 == retcode)
        {
            get_user_friends();
            get_group_name_list();
            debug_info("Login Sucess ... (%s,%d)", __FILE__, __LINE__);
        }
        else
        {
            debug_info("Login Failed with error code %d ... (%s,%d)", retcode, __FILE__, __LINE__);
            return false;
        }

    }
    catch(...)
    {
        debug_error("Cant not parse json body ... (%s,%d)", \
                    __FILE__, __LINE__);
        return false;
    }
    return true;
}

void QQPlugin::get_user_friends()
{
    std::string uri ="http://s.web2.qq.com/api/get_user_friends2";

    std::list<std::string> headers;
    headers.push_back("Referer: http://s.web2.qq.com/proxy.html?v=20101025002");

    std::vector<curlpp::OptionBase*> settings;
    settings.push_back(new curlpp::options::HttpHeader(headers));
    HttpClient * request = Singleton<HttpClient>::getInstance();
    request->setOptions(settings);

    std::string body ="r=%7B%22h%22%3A%22hello%22%2C%22vfwebqq%22%3A%22"+\
                      vfwebqq+"%22%7D";

    std::string result = request->requestServer(uri, body);

    try
    {
        Json::Reader jsonReader;
        Json::Value root;
        jsonReader.parse(result, root, false);
        int retcode = root["retcode"].asInt();
        if ( 0 == retcode)
        {
            parse_user_friends(root);
            debug_info("Get friends list Success ... (%s,%d)", __FILE__, __LINE__);
        }
        else
        {
            debug_info("Get friends list  failed with error code %d ... (%s,%d)", retcode, __FILE__, __LINE__);
        }

    }catch(...)
    {
        debug_error("Cant not parse json body ... (%s,%d)",\
                    __FILE__, __LINE__);
    }
}

void QQPlugin::parse_user_friends(const Json::Value & root)
{
    try
    {
        Json::Value value = root["result"];
        Json::Value cates = value["categories"];
        Json::Value friends = value["friends"];
        Json::Value vipinfo = value["vipinfo"];
        for( Json::Value::iterator it = cates.begin(); it != cates.end(); it ++)
        {
            QQCategory cate;
            cate.index =  (*it)["index"].asInt();
            cate.name = (*it)["name"].asString();
            res->categories[ cate.index] = cate;
        }

        Json::FastWriter writer ;

        for( Json::Value::iterator it = friends.begin(); it != friends.end(); it ++ )
        {
            QQBuddy buddy;
            buddy.cate_index = (*it)["categories"].asInt();
            buddy.uin = writer.write((*it)["uin"]);
            buddy.flag = writer.write((*it)["flag"]);
            res->contacts[buddy.uin] = buddy;
        }

        for( Json::Value::iterator it = vipinfo.begin(); it != vipinfo.end(); it ++)
        {
            std::string u = writer.write((*it)["u"]);
            if( res->contacts.count(u) == 0 )
            {
                debug_error("uin %s  does not exist ... (%s,%d)", u.c_str(),
                            __FILE__, __LINE__);
                continue ;
            }
            else
            {
                res->contacts[u].is_vip = (*it)["is_vip"].asInt();
                res->contacts[u].vip_level = (*it)["vip_level"].asInt();
            }
        }
    }
    catch(...){
        debug_error("Fail to parse user friends ... (%s,%d)", __FILE__, __LINE__);
    }
}



void QQPlugin::get_group_name_list()
{
    std::string uri="http://s.web2.qq.com/api/get_group_name_list_mask2";

    std::string body="r=%7B%22vfwebqq%22%3A%22"+\
                     vfwebqq+"%22%7D";

    HttpClient *request = Singleton<HttpClient>::getInstance();
    std::list<std::string> headers;

    headers.push_back("Referer: http://s.web2.qq.com/proxy.html?v=20110412001");

    std::vector<curlpp::OptionBase*> settings;
    settings.push_back(new curlpp::options::HttpHeader(headers));
    request->setOptions(settings);
    std::string result = request->requestServer(uri, body);
    try
    {
        Json::FastWriter writer;
        Json::Reader jsonReader;
        Json::Value root;
        jsonReader.parse(result, root, false);
        int retcode = root["retcode"].asInt();
        if ( 0 == retcode)
        {
            Json::Value gnamelist = root["gnamelist"];
            for( Json::Value::iterator it = gnamelist.begin();  it != gnamelist.end(); it ++)
            {
                QQGroup group;
                group.name = writer.write((*it)["name"]);
                group.gid = writer.write((*it)["gid"]);
                group.flag = writer.write((*it)["flag"]);
                group.code = writer.write((*it)["code"]);
                res->groups[group.name]= group;
            }

            debug_info("Get group name list Success ... (%s,%d)", __FILE__, __LINE__);
        }
        else
        {
            debug_info("Get group  list  failed with error code %d ... (%s,%d)", retcode, __FILE__, __LINE__);
        }

    }catch(...)
    {
        debug_error("Cant not parse json body ... (%s,%d)", \
                    __FILE__, __LINE__);
    }
}
