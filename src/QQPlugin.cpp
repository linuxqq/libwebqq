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
#include <algorithm>
#include "QQTask.h"

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
    ThreadPool::sync_all();
    ThreadPool::done();
    delete res;
}

bool QQPlugin::webqq_login(const std::string & user, const std::string & password, const std::string & status)
{

    QQBuddy me;
    me.uin = user;
    me.status = status;
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

    QQUtil::Tokenize(body, vec, "\',\'");
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
            get_online_buddies();
            get_group_name_list();
            get_group_info();
            //ThreadPool::sync_all();
            debug_info("Login Sucess ... (%s,%d)", __FILE__, __LINE__);
            GetMiscInfo *get_misc_info = new GetMiscInfo(vfwebqq);
            ThreadPool::run(get_misc_info, res, true);

            std::string body ="r=%7B%22clientid%22%3A%22"+clientid+    \
                              "%22%2C%22psessionid%22%3A%22"+psessionid\
                              +"%22%2C%22key%22%3A0%2C%22ids%22%3A%5B%5D%7D&clientid="+\
                              clientid+"&psessionid="+ psessionid;


            Poll2 * poll = new Poll2(body );
            ThreadPool::run(poll, res, true);

            return true;

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
                group.name = QQUtil::trim(writer.write((*it)["name"]));
                QQUtil::replaceAll(group.name,"\"","");
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


void QQPlugin::get_online_buddies()
{
    std::string uri ="http://d.web2.qq.com/channel/get_online_buddies2?clientid="+\
                     clientid +"&psessionid="+ psessionid +"&t=1339907056293";

    std::list<std::string> headers;
    headers.push_back("Referer: http://d.web2.qq.com/proxy.html");

    HttpClient * request = Singleton<HttpClient>::getInstance();

    request->setHttpHeaders(headers);

    std::string result = request->requestServer(uri);

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

void QQPlugin::get_group_info()
{
    debug_info("Get Group Info ... (%s,%d)", __FILE__, __LINE__);
    //get_group_name_list();
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

        root["to"] = static_cast<Json::Value::UInt64> (QQUtil::StrToInt(uin));
        root["face"]=0;
        content[0] = message_body;
        content[1][0]="font";
        content[1][1] = font;

        std::string str = writer.write(content);
        size_t pos = str.find_last_of("]");
        str.erase(str.begin()+ pos+1, str.end());

        root["msg_id"] = message_id ++ ;
        root["clientid"] = static_cast<Json::Value::UInt64>(QQUtil::StrToInt(clientid));
        root["psessionid"] = psessionid;

        root["content"] = str;
        std::string body = writer.write(root);

        pos = body.find_last_of("}");
        body.erase(body.begin()+ pos+1, body.end());

        std::string from = "\"\\\"";
        std::string to ="\\\"";

        QQUtil::replaceAll( body, from, to);

        from = "\\\"\"";
        to = "\\\"";

        QQUtil::replaceAll( body, from, to);

        body = QQUtil::urlencode(body);
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
        root["clientid"] = static_cast<Json::Value::UInt64> (QQUtil::StrToInt(clientid));
        root["psessionid"] = psessionid;

        root["content"] = str;
        std::string body = writer.write(root);

        pos = body.find_last_of("}");
        body.erase(body.begin()+ pos+1, body.end());

        std::string from = "\"\\\"";
        std::string to ="\\\"";

        QQUtil::replaceAll( body, from, to);

        from = "\\\"\"";
        to = "\\\"";

        QQUtil::replaceAll( body, from, to);

        body = QQUtil::urlencode(body);
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

bool QQPlugin::send_buddy_nudge(const std::string & uin)
{
    bool success = false;

    SendShake * job = new SendShake(uin, clientid, psessionid);
    ThreadPool::run(job, & success, true);
    ThreadPool::sync(job);
    return success;
}
