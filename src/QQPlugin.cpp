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
#include <algorithm>
using namespace QQUtil;


int QQPlugin::message_id = 70480000;

ResourceManager::ResourceManager()
{

}

ResourceManager::~ResourceManager()
{

}

void ResourceManager::lock()
{
    rw_mutex.lock();
}

void ResourceManager::ulock()
{
    rw_mutex.unlock();
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

    QQBuddy me;
    me.uin = user;
    me.cate_index = 0;
    res->contacts[me.uin] = me;

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

    request->setHttpHeaders(headers);

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
            ThreadPool::sync_all();
            get_online_buddies();
            get_group_info();
            ThreadPool::sync_all();
            debug_info("Login Sucess ... (%s,%d)", __FILE__, __LINE__);

            std::string body ="r=%7B%22clientid%22%3A%22"+clientid+    \
                              "%22%2C%22psessionid%22%3A%22"+psessionid\
                              +"%22%2C%22key%22%3A0%2C%22ids%22%3A%5B%5D%7D&clientid="+\
                              clientid+"&psessionid="+ psessionid;

            std::cout<<body<<std::endl;

            Poll2 * poll = new Poll2(body );
            ThreadPool::run(poll, res, true);

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

    HttpClient * request = Singleton<HttpClient>::getInstance();

    request->setHttpHeaders(headers);

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

                GetFriendUin * getuin = new GetFriendUin( it->first, vfwebqq);
                ThreadPool::run(getuin, res, true);

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
        Json::FastWriter writer ;

        for( Json::Value::iterator it = cates.begin(); it != cates.end(); it ++)
        {
            QQCategory cate;
            cate.index =  (*it)["index"].asInt();
            cate.name = (*it)["name"].asString();
            res->categories[ cate.index] = cate;
        }


        for( Json::Value::iterator it = friends.begin(); it != friends.end(); it ++ )
        {
            QQBuddy buddy;
            buddy.cate_index = (*it)["categories"].asInt();
            buddy.uin = QQUtil::trim(writer.write((*it)["uin"]));
            res->contacts[buddy.uin] = buddy;
        }

        for( Json::Value::iterator it = vipinfo.begin(); it != vipinfo.end(); it ++)
        {
            std::string u = QQUtil::trim(writer.write((*it)["u"]));
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

    debug_info("Get Group name list (%s,%d)", __FILE__, __LINE__);

    std::string uri="http://s.web2.qq.com/api/get_group_name_list_mask2";

    std::string body="r=%7B%22vfwebqq%22%3A%22"+\
                     vfwebqq+"%22%7D";

    HttpClient *request = Singleton<HttpClient>::getInstance();
    std::list<std::string> headers;

    headers.push_back("Referer: http://s.web2.qq.com/proxy.html");

    request->setHttpHeaders(headers);

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
            Json::Value gnamelist = root["result"]["gnamelist"];
            res->lock();
            for( Json::Value::iterator it = gnamelist.begin();  it != gnamelist.end(); it ++)
            {
                QQGroup group;
                group.name = writer.write((*it)["name"]);
                group.gid = QQUtil::trim(writer.write((*it)["gid"]));
                group.flag = writer.write((*it)["flag"]);
                group.code = QQUtil::trim(writer.write((*it)["code"]));
                res->groups[group.code]= group;
            }
            if ( res->groups.empty())
            {
                debug_info("No group list... (%s,%d)", __FILE__, __LINE__);
            }
            res->ulock();
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

    request->setHttpHeaders(headers);

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
            res->contacts[uin].lnick= QQUtil::trim(writer.write( root["result"][0]["lnick"]));
        }
        else
        {
            debug_info("Get long  nick failed with error code %d ... (%s,%d)", retcode, __FILE__, __LINE__);
        }

    }catch(...)
    {
        res->ulock();
        debug_error("Failed to parse json content... (%s,%d)", __FILE__, __LINE__);
    }
    res->ulock();

    delete request;
}

QQPlugin::GetFriendUin::GetFriendUin( const std::string & uin, const std::string & vfwebqq)
{
    this->uin = uin;
    this->vfwebqq = vfwebqq;
}

void QQPlugin::GetFriendUin::run( void * ptr)
{
    ResourceManager *res = reinterpret_cast < ResourceManager *>(ptr);
    std::string temp_uin = uin;
    std::string::size_type p = temp_uin.find_last_of('\n');
    if(p != std::string::npos) temp_uin.erase(p);

    HttpClient *request = new HttpClient();
    std::string uri = "http://s.web2.qq.com/api/get_friend_uin2?tuin="+\
                      temp_uin + "&verifysession=&type=1&code=&vfwebqq="+\
                      vfwebqq;

    std::list<std::string> headers;
    headers.push_back("Referer: http://s.web2.qq.com/proxy.html?v=20110412001&callback=1&id=1");

    request->setHttpHeaders(headers);

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
            res->contacts[uin].qqnumber=  QQUtil::trim(writer.write(root["result"]["account"]));
        }
        else
        {
            debug_info("Get friend uin failed with error code %d ... (%s,%d)", retcode, __FILE__, __LINE__);
        }

    }catch(...)
    {
        res->ulock();
        debug_error("Failed to parse json content... (%s,%d)", __FILE__, __LINE__);
    }

    res->ulock();
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

    request->setHttpHeaders(headers);

    std::string result = request->requestServer(uri);
    try
    {
        res->lock();
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
            res->contacts[uin].nick= QQUtil::trim(writer.write( root["result"]["nick"]));
            res->contacts[uin].gender= writer.write( root["result"]["gender"]);
            res->contacts[uin].email= writer.write( root["result"]["email"]);
            res->contacts[uin].shengxiao = root["result"]["shengxiao"].asInt();
            res->contacts[uin].mobile= writer.write( root["result"]["mobile"]);
        }
        else
        {
            debug_info("Get friends info2 failed with error code %d ... (%s,%d)", retcode, __FILE__, __LINE__);
        }
        res->ulock();

    }catch(...)
    {
        res->ulock();
        debug_error("Cant not parse json body ... (%s,%d)", \
                    __FILE__, __LINE__);
    }
}

void QQPlugin::get_online_buddies()
{
    std::string uri ="http://d.web2.qq.com/channel/get_online_buddies2?clientid="+\
                     clientid +"&psessionid="+ psessionid +"&t=1339907056293";

    std::list<std::string> headers;
    headers.push_back("Referer: http://d.web2.qq.com/proxy.html");

    HttpClient * request = Singleton<HttpClient>::getInstance();

    request->setHttpHeaders(headers);

    std::string result = request->requestServer(uri);

    std::cout<<result<<std::endl;

    try{
        Json::FastWriter writer;
        Json::Reader jsonReader;
        Json::Value root;
        jsonReader.parse(result, root, false);
        int retcode = root["retcode"].asInt();
        if ( 0 == retcode)
        {
            for( Json::Value::iterator it = root["result"].begin();
                 it != root["result"].end() ; it ++)
            {
                res ->lock();
                std::string u= QQUtil::trim(writer.write((*it)["uin"]));
                if ( res->contacts.count(u) != 0 )
                {
                    res->contacts[u].status = QQUtil::trim((*it)["status"].asString());
                    res->contacts[u].client_type = (*it)["client_type"].asInt();
                }
                else{
                    debug_error("Invalid Uin ... %s ( %s, %d)", u.c_str(), __FILE__, __LINE__);
                }
                res->ulock();
            }
        }
        else{
            debug_error("Get online buddies failed with error code %d ... (%s,%d)", retcode, __FILE__, __LINE__);
        }
    }catch(...)
    {
        debug_error("Cant not parse json body ... (%s,%d)", \
                    __FILE__, __LINE__);
    }

}

QQPlugin::GetGroupInfo::GetGroupInfo(const std::string & gcode, const std::string & vfwebqq)
{
    this->gcode = gcode;
    this->vfwebqq = vfwebqq;
}

void QQPlugin::GetGroupInfo::run( void *ptr)
{
    ResourceManager *res = reinterpret_cast < ResourceManager *>(ptr);
    std::string temp_gcode = gcode;
    std::string::size_type p = temp_gcode.find_last_of('\n');
    if(p != std::string::npos) temp_gcode.erase(p);

    HttpClient *request = new HttpClient();
    std::string uri = "http://s.web2.qq.com/api/get_group_info?gcode=%5B"+
                      temp_gcode+"%5D&retainKey=memo%2Cgcode&vfwebqq="+vfwebqq + \
                      "&t=1339476483796";

    std::list<std::string> headers;
    headers.push_back("Referer: http://s.web2.qq.com/proxy.html?v=20110412001&callback=1&id=3");
    request->setHttpHeaders(headers);
    std::string result = request->requestServer(uri);
    delete request;
    try
    {
        Json::FastWriter writer;
        Json::Reader jsonReader;
        Json::Value root;
        jsonReader.parse(result, root, false);
        int retcode = root["retcode"].asInt();
        if ( 0 == retcode)
        {
            res->lock();
            res->groups[gcode].memo = QQUtil::trim(root["result"][0]["memo"].asString());
            res->ulock();
        }
        else{
            debug_error("Get Group memo fail...(%s,%d)", __FILE__, __LINE__);
        }
    }
    catch(...)
    {
        debug_error("Cant not parse json body ... (%s,%d)", \
                    __FILE__, __LINE__);
    }

    uri = "http://s.web2.qq.com/api/get_group_info_ext2?gcode="+\
          temp_gcode+ "&vfwebqq="+ vfwebqq+"&t=1339476485660";

    request = new HttpClient();
    headers.clear();
    headers.push_back("Referer: http://s.web2.qq.com/proxy.html?v=20110412001&callback=1&id=3");
    request->setHttpHeaders(headers);

    result = request->requestServer(uri);
    delete request;
    try{
        Json::FastWriter writer;
        Json::Reader jsonReader;
        Json::Value root;
        jsonReader.parse(result, root, false);
        int retcode = root["retcode"].asInt();

        if ( 0 == retcode)
        {
            res->lock();
            Json::Value ginfo = root["result"]["ginfo"];
            gcode = QQUtil::trim( writer.write(ginfo["code"]));
            if ( res->groups.count(gcode) == 0)
            {
                debug_error( "invalid group code. ...(%s,%d)", __FILE__, __LINE__);
            }
            res->groups[gcode].gid = QQUtil::trim(writer.write(ginfo["gid"]));
            res->groups[gcode].level = ginfo["level"].asInt();
            res->groups[gcode].fingermemo = ginfo["fingermemo"].asString();
            res->groups[gcode].flag = writer.write(ginfo["flag"]);
            res->groups[gcode].gclass = QQUtil::trim(writer.write(ginfo["class"]));
            res->groups[gcode].name = writer.write(ginfo["name"]);
            res->groups[gcode].owner = writer.write(ginfo["owner"]);
            res->groups[gcode].option = ginfo["option"].asInt();

            Json::Value minfo = root["result"]["minfo"];
            for ( Json::Value::iterator it = minfo.begin(); it != minfo.end() ; it ++)
            {
                std::string uin = QQUtil::trim( writer.write((*it)["uin"]));
                QQBuddy buddy;
                buddy.uin = uin;
                buddy.city = (*it)["city"].asString();
                buddy.country = (*it)["country"].asString();
                buddy.nick = (*it)["nick"].asString();
                buddy.province = (*it)["province"].asString();
                res->group_contacts[gcode][uin] = buddy;
            }

            Json::Value status = root["result"]["status"];
            for ( Json::Value::iterator it = status.begin(); it != status.end() ; it ++)
            {
                std::string uin = QQUtil::trim(writer.write((*it)["uin"]));
                if ( res->group_contacts[gcode].count(uin) == 0)
                {
                    debug_error("Invalid group member uin ... (%s,%d)", __FILE__, __LINE__);
                    continue;
                }
                res->group_contacts[gcode][uin].client_type = (*it)["client_type"].asInt();
                int status = (*it)["stat"].asInt();
                switch ( status)
                {
                case 10:
                    res->group_contacts[gcode][uin].status = "online";
                    break ;
                case 30:
                    res->group_contacts[gcode][uin].status = "away";
                    break;
                case 70 :
                    res->group_contacts[gcode][uin].status = "offline";
                    break;
                }
            }
            res->ulock();
        }
        else{
            debug_error("Get Group member list fail");
        }
    }catch(...)
    {
        res->ulock();
        debug_error("Cant not parse json body ... (%s,%d)", \
                    __FILE__, __LINE__);
    }
}

void QQPlugin::get_group_info()
{
    debug_info("Get Group Info ... (%s,%d)", __FILE__, __LINE__);
    get_group_name_list();
    if ( res->groups.empty())
    {
        debug_error("Empty Group list!");
    }
    for( std::map<std::string, QQGroup>::iterator it = res->groups.begin();
         it != res->groups.end(); it ++)
    {
        GetGroupInfo * job = new GetGroupInfo( (*it).first, vfwebqq);
        ThreadPool::run( job, res, true);
    }
}

QQPlugin::Poll2::Poll2(const std::string & data)
{
    this->body = data;
}

void QQPlugin::Poll2::run( void * ptr)
{
    while(1)
    {
        HttpClient * client;
        client = new HttpClient();
        std::list<std::string> headers;

        headers.push_back("Referer: http://d.web2.qq.com/proxy.html");

        client->setHttpHeaders(headers);

        std::string result = client->requestServer("http://d.web2.qq.com/channel/poll2",body);
        std::cout<<result<<std::endl;
        delete client;
        Json::Reader jsonReader;
        Json::Value root;
        jsonReader.parse(result, root, false);
        int ret= root["retcode"].asInt();
        if ( ret == 102 )
        {
            continue;
        }
        else if ( ret == 103)
        {
            debug_error("lost connection.");
            break;
        }
        else if (ret == 0 )
        {
            ResourceManager * res  = reinterpret_cast<ResourceManager *> (ptr);
            res->lock();

            for ( Json::Value::iterator iter = root["result"].begin() ; iter != root["result"].end() ; iter ++ )
            {

                Json::FastWriter writer;
                std::string value = QQUtil::trim(writer.write((*iter)["value"]));

                std::string poll_type  = (*iter)["poll_type"].asString();
                if ( poll_type == "message")
                {
#ifdef USE_EVENT_QUEUE
                    res->event_queue.push_back(std::make_pair<QQEvent, std::string>(ON_BUDDY_MESSAGE, value));
#endif
                    if ( res->event_adapter.is_event_registered(ON_BUDDY_MESSAGE) )
                    {
                        res->event_adapter.trigger(ON_BUDDY_MESSAGE ,value);
                    }
                    else
                    {
                        debug_info( " No on message event adapter loaded. (%s,%d)", __FILE__, __LINE__);
                    }
                }

                else if (poll_type == "buddies_status_change")
                {
#ifdef USE_EVENT_QUEUE

                    res->event_queue.push_back(std::make_pair<QQEvent, std::string>(ON_BUDDY_STATUS_CHANGE, value));
#endif
                     if ( res->event_adapter.is_event_registered(ON_BUDDY_STATUS_CHANGE) )
                    {
                        res->event_adapter.trigger(ON_BUDDY_STATUS_CHANGE ,value);
                    }
                    else
                    {
                        debug_info( " No on message event adapter loaded. (%s,%d)", __FILE__, __LINE__);
                    }
                }
                else if ( poll_type == "group_message")
                {
#ifdef USE_EVENT_QUEUE
                    res->event_queue.push_back(std::make_pair<QQEvent, std::string>( ON_GROUP_MESSAGE , value));
#endif
                    if ( res->event_adapter.is_event_registered (ON_GROUP_MESSAGE))
                    {
                        res->event_adapter.trigger(ON_GROUP_MESSAGE ,value);
                    }
                    else
                    {
                        debug_info( " No on message event adapter loaded. (%s,%d)", __FILE__, __LINE__);
                    }
                }
            }
            res->ulock();
        }
    }
}

QQPlugin::SendBuddyMessage::SendBuddyMessage(const std::string & body)
{
    this->body = body;
}

void QQPlugin::SendBuddyMessage::run(void * ptr)
{
    bool  *success =  (bool*)(ptr);
    HttpClient * client;
    client = new HttpClient();
    std::list<std::string> headers;

    headers.push_back("Referer: http://d.web2.qq.com/proxy.html?v=20110331002&callback=1&id=3");

    client->setHttpHeaders(headers);

    std::cout<<body<<std::endl;

    std::string result = client->requestServer("http://d.web2.qq.com/channel/send_buddy_msg2",body);
    delete client;
    try{
        Json::Reader jsonReader;
        Json::Value root;
        jsonReader.parse(result, root, false);
        int ret= root["retcode"].asInt();
        if ( ret == 0)
        {
            *success = true;
            debug_info("Success to send buddy message!... (%s,%d)", __FILE__, __LINE__);
        }
        else
        {
            *success = false;
            debug_error("Fail to send buddy message!");
        }
    }catch(...)
    {
        debug_error("fail to parse json content ...(%s,%d)", __FILE__, __LINE__);
    }
}

QQPlugin::SendGroupMessage::SendGroupMessage(const std::string & body )
{
    this->body = body;
}

void QQPlugin::SendGroupMessage::run( void *ptr)
{
     bool  *success =  (bool*)(ptr);
    HttpClient * client;
    client = new HttpClient();
    std::list<std::string> headers;

    headers.push_back("Referer: http://d.web2.qq.com/proxy.html?v=20110331002&callback=1&id=2");

    client->setHttpHeaders(headers);

    std::cout<<body<<std::endl;

    std::string result = client->requestServer("http://d.web2.qq.com/channel/send_qun_msg2",body);
    delete client;
    try{
        Json::Reader jsonReader;
        Json::Value root;
        jsonReader.parse(result, root, false);
        int ret= root["retcode"].asInt();
        if ( ret == 0)
        {
            *success = true;
            debug_info("Success to send group message!... (%s,%d)", __FILE__, __LINE__);
        }
        else
        {
            *success = false;
            debug_error("Fail to send buddy message!");
        }
    }catch(...)
    {
        debug_error("fail to parse json content ...(%s,%d)", __FILE__, __LINE__);
    }
}

bool QQPlugin::send_buddy_message(const std::string & uin, const std::string & message_body)
{
    try
    {

        Json::Value root;
        Json::Value font;
        Json::Value content;
        Json::FastWriter writer;

        font["name"] ="宋体";
        font["size"] = "16";
        for( int i =0 ; i < 3 ; i ++)
            font["style"][i] = 0;
        font["color"] ="000000";

        root["to"] = StrToInt(uin);
        root["face"]=0;
        content[0] = message_body;
        content[1][0]="font";
        content[1][1] = font;

        std::string str = writer.write(content);
        size_t pos = str.find_last_of("]");
        str.erase(str.begin()+ pos+1, str.end());

        root["msg_id"] = message_id ++ ;
        root["clientid"] = QQUtil::StrToInt(clientid);
        root["psessionid"] = psessionid;

        root["content"] = str;
        std::string body = writer.write(root);

        pos = body.find_last_of("}");
        body.erase(body.begin()+ pos+1, body.end());

        size_t start_pos;
        std::string from = "\"\\\"";
        std::string to ="\\\"";

        QQUtil::replaceAll( body, from, to);

        from = "\\\"\"";
        to = "\\\"";

        QQUtil::replaceAll( body, from, to);

        body = urlencode(body);
        std::string::size_type p = body.find_last_of('\n');
        if(p != std::string::npos) body.erase(p);
        body = "r=" + body;
        body +="&clientid=" + clientid + "&psessionid=" + psessionid;

        bool sucess =false ;

        SendBuddyMessage * job= new SendBuddyMessage( body);

        ThreadPool::run(job, &sucess, true);
        ThreadPool::sync(job);
        return sucess;

    }catch(...)
    {
        debug_error("Failt to parse json content... (%s,%d)", __FILE__, __LINE__);
        return false;
    }
}


bool QQPlugin::send_group_message(const std::string & group_class, const std::string & message_body)
{
    res->lock();
    bool exists = false;
    std::string g_uin ;
    for ( std::map<std::string , QQGroup>::iterator it = res->groups.begin() ;
          it != res->groups.end();  it ++)
    {
        std::string g_class = it -> first;
        std::string::size_type p = g_class.find_last_of('\n');
        if(p != std::string::npos) g_class.erase(p);

        if ( g_class == group_class)
        {
            exists = true;
            g_uin = it->second.gid;
            std::string::size_type p = g_uin.find_last_of('\n');
            if(p != std::string::npos) g_uin.erase(p);
        }
    }
    if ( ! exists)
    {
        debug_error("Invalid group class string... (%s,%d)", __FILE__, __LINE__);
        res->ulock();
        return false;
    }
    res->ulock();

    try
    {

        Json::Value root;
        Json::Value font;
        Json::Value content;
        Json::FastWriter writer;

        font["name"] ="宋体";
        font["size"] = "16";
        for( int i =0 ; i < 3 ; i ++)
            font["style"][i] = 0;
        font["color"] ="000000";

        root["group_uin"] = g_uin;
        root["face"]=0;
        content[0] = message_body;
        content[1][0]="font";
        content[1][1] = font;

        std::string str = writer.write(content);
        size_t pos = str.find_last_of("]");
        str.erase(str.begin()+ pos+1, str.end());

        root["msg_id"] = message_id ++ ;
        root["clientid"] = QQUtil::StrToInt(clientid);
        root["psessionid"] = psessionid;

        root["content"] = str;
        std::string body = writer.write(root);

        pos = body.find_last_of("}");
        body.erase(body.begin()+ pos+1, body.end());

        size_t start_pos;
        std::string from = "\"\\\"";
        std::string to ="\\\"";

        QQUtil::replaceAll( body, from, to);

        from = "\\\"\"";
        to = "\\\"";

        QQUtil::replaceAll( body, from, to);

        body = urlencode(body);
        std::string::size_type p = body.find_last_of('\n');
        if(p != std::string::npos) body.erase(p);
        body = "r=" + body;
        body +="&clientid=" + clientid + "&psessionid=" + psessionid;

        bool sucess =false ;

        SendGroupMessage * job= new SendGroupMessage( body);

        ThreadPool::run(job, &sucess, true);
        ThreadPool::sync(job);
        return sucess;

    }catch(...)
    {
        debug_error("Failt to parse json content... (%s,%d)", __FILE__, __LINE__);
        return false;
    }
}
