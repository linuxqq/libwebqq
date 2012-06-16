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
#include "ThreadPool.h"

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
    ThreadPool::init(8);
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
    settings.push_back(new curlpp::options::NoSignal(1));

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
            ThreadPool::sync_all();
            debug_info("Login Sucess ... (%s,%d)", __FILE__, __LINE__);

            std::string body ="r=%7B%22clientid%22%3A%22"+clientid+    \
                  "%22%2C%22psessionid%22%3A%22"+psessionid\
                  +"%22%2C%22key%22%3A0%2C%22ids%22%3A%5B%5D%7D&clientid="+\
                  clientid+"&psessionid="+ psessionid;

            std::cout<<body<<std::endl;

            Poll2 * poll = new Poll2(body );
            ThreadPool::run(poll, NULL, true);

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
    settings.push_back(new curlpp::options::NoSignal(1));
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
            for( std::map<std::string , QQBuddy>::iterator it = res->contacts.begin();
                 it != res->contacts.end(); it ++)
            {
                GetLongNick*  getnick = new GetLongNick( it->first, vfwebqq);
                ThreadPool::run(getnick,res, true);

                GetFriendsInfo2 * getinfo = new GetFriendsInfo2( it->first, vfwebqq);
                ThreadPool::run(getinfo, res, true);
            }

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
    settings.push_back(new curlpp::options::NoSignal(1));
    request->setOptions(settings);
    std::string result = request->requestServer(uri, body);

    delete request;
    request = NULL;

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

QQPlugin::GetLongNick::GetLongNick( const std::string & uin, const std::string & vfwebqq )
{
    this->uin = uin;
    this->vfwebqq = vfwebqq;
}

void QQPlugin::GetLongNick::run(void *ptr)
{

    ResourceManager *res = reinterpret_cast < ResourceManager *>(ptr);
    std::string temp_uin = uin;
    std::string::size_type p = temp_uin.find_last_of('\n');
    if(p != std::string::npos) temp_uin.erase(p);

    HttpClient *request = new HttpClient();
    std::string uri = "http://s.web2.qq.com/api/get_single_long_nick2?tuin="+\
                      temp_uin + "&vfwebqq="+ vfwebqq;

    std::list<std::string> headers;
    headers.push_back("Referer: http://s.web2.qq.com/proxy.html?v=20110412001&callback=1&id=3");

    std::vector<curlpp::OptionBase*> settings;
    settings.push_back(new curlpp::options::HttpHeader(headers));
    settings.push_back(new curlpp::options::NoSignal(1));
    request->setOptions(settings);
    std::string result = request->requestServer(uri);
    res->lock();

    try{

        Json::FastWriter writer;
        Json::Reader jsonReader;
        Json::Value root;
        jsonReader.parse(result, root, false);
        int retcode = root["retcode"].asInt();
        if ( 0 == retcode)
        {
            res->contacts[uin].lnick= writer.write( root["result"][0]["lnick"]);
        }
        else
        {
            debug_info("Get long  nick failed with error code %d ... (%s,%d)", retcode, __FILE__, __LINE__);
        }

    }catch(...)
    {
        debug_error("Failed to parse json content... (%s,%d)", __FILE__, __LINE__);
    }
    res->ulock();
    delete request;
}

QQPlugin::GetFriendsInfo2::GetFriendsInfo2( const std::string & uin, const std::string & vfwebqq )
{
    this-> uin = uin;
    this->vfwebqq = vfwebqq ;
}

void QQPlugin::GetFriendsInfo2::run( void * ptr)
{
     ResourceManager *res = reinterpret_cast < ResourceManager *>(ptr);
    std::string temp_uin = uin;
    std::string::size_type p = temp_uin.find_last_of('\n');
    if(p != std::string::npos) temp_uin.erase(p);

    HttpClient *request = new HttpClient();
    std::string uri = "http://s.web2.qq.com/api/get_friend_info2?tuin="+\
                      temp_uin + "&vfwebqq="+ vfwebqq;

    std::list<std::string> headers;
    headers.push_back("Referer: http://s.web2.qq.com/proxy.html?v=20110412001&callback=1&id=3");

    std::vector<curlpp::OptionBase*> settings;
    settings.push_back(new curlpp::options::HttpHeader(headers));
    settings.push_back(new curlpp::options::NoSignal(1));
    request->setOptions(settings);
    std::string result = request->requestServer(uri);
    res->lock();
    debug_info("GetFriendsInfo: %s", result.c_str());
    try
    {
        Json::FastWriter writer;
        Json::Reader jsonReader;
        Json::Value root;
        jsonReader.parse(result, root, false);
        int retcode = root["retcode"].asInt();
        if ( 0 == retcode)
        {
            Birthday birthday ;
            birthday.year = root["result"]["Birthday"]["year"].asInt();
            birthday.month = root["result"]["Birthday"]["month"].asInt();
            birthday.month = root["result"]["Birthday"]["day"].asInt();
            res->contacts[uin].birthday = birthday;
            res->contacts[uin].blood = root["result"]["blood"].asInt();
            //res->contacts[uin].face= writer.write( root["result"]["face"]);
            res->contacts[uin].occupation= writer.write( root["result"]["occupation"]);
            res->contacts[uin].phone= writer.write( root["result"]["phone"]);
            res->contacts[uin].allow= root["result"]["allow"].asInt();
            res->contacts[uin].college= writer.write( root["result"]["college"]);
            res->contacts[uin].constel=  root["result"]["constel"].asInt();
            res->contacts[uin].homepage= writer.write( root["result"]["homepage"]);
            res->contacts[uin].country= writer.write( root["result"]["country"]);
            res->contacts[uin].city= writer.write( root["result"]["city"]);
            res->contacts[uin].province = writer.write( root["result"]["province"]);
            res->contacts[uin].personal= writer.write( root["result"]["personal"]);
            res->contacts[uin].nick= writer.write( root["result"]["nick"]);
            res->contacts[uin].gender= writer.write( root["result"]["gender"]);
            res->contacts[uin].email= writer.write( root["result"]["email"]);
            res->contacts[uin].shengxiao = root["result"]["shengxiao"].asInt();
            res->contacts[uin].mobile= writer.write( root["result"]["mobile"]);
        }
        else
        {
            debug_info("Get friends info2 failed with error code %d ... (%s,%d)", retcode, __FILE__, __LINE__);
        }

    }catch(...)
    {
        debug_error("Cant not parse json body ... (%s,%d)", \
                    __FILE__, __LINE__);
    }

    res->ulock();
}

QQPlugin::Poll2::Poll2(const std::string & data)
{
    this->body = data;
}

void QQPlugin::Poll2::run( void *)
{
    while(1)
    {
        HttpClient * client;
        client = new HttpClient();
        std::list<std::string> headers;

        headers.push_back("Referer: http://d.web2.qq.com/proxy.html");
        std::vector<curlpp::OptionBase*> settings;
        settings.push_back(new curlpp::options::HttpHeader(headers));
        settings.push_back(new curlpp::options::NoSignal(1));
        client->setOptions(settings);
        std::string result = client->requestServer("http://d.web2.qq.com/channel/poll2",body);
        std::cout<<result<<std::endl;

        Json::Reader jsonReader;
        Json::Value root;
        jsonReader.parse(result, root, false);
        int ret= root["retcode"].asInt();
        if ( ret == 103)
        {
            debug_info("lost connection.");
            break;
        }
        delete client;
    }
}
