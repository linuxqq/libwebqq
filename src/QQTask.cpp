/**
 * @file   QQTask.cpp
 * @author Xiang Wang <xiang_wang@trendmicro.com.cn>
 * @date   Wed Jul 25 17:26:34 2012
 *
 * @brief
 *
 *
 */

#include "QQTask.h"
#include "QQDebug.h"
#include "QQTypes.h"
#include "json/json.h"
#include "QQUtil.h"
#include "QQAuthentication.h"
#include <iostream>
#include "HttpClient.h"
#include "QQPlugin.h"

QQTask::QQTask(const std::string & uin, const std::string &vfwebqq)
{
    this->uin = uin;
    this->vfwebqq = vfwebqq;
}

QQTask::~QQTask(){}

GetLongNick::GetLongNick( const std::string & uin, const std::string & vfwebqq ):QQTask(uin, vfwebqq)
{
}

void GetLongNick::run(void *ptr)
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
            std::string lnick = QQUtil::trim(writer.write( root["result"][0]["lnick"]));
            QQUtil::replaceAll(lnick,"\"","");
            res->contacts[uin].lnick = lnick;
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


GetFriendUin::GetFriendUin( const std::string & uin, const std::string & vfwebqq):QQTask(uin, vfwebqq)
{

}

void GetFriendUin::run( void * ptr)
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

GetFriendsInfo2::GetFriendsInfo2( const std::string & uin, const std::string & vfwebqq ):QQTask(uin, vfwebqq)
{

}

void GetFriendsInfo2::run( void * ptr)
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
            std::string nick = QQUtil::trim(writer.write( root["result"]["nick"]));
            QQUtil::replaceAll(nick,"\"","");
            res->contacts[uin].nick= nick;
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


GetGroupInfo::GetGroupInfo(const std::string & uin, const std::string & vfwebqq):QQTask(uin, vfwebqq)
{
    this->gcode = uin;
}

void GetGroupInfo::run( void *ptr)
{
    ResourceManager *res = reinterpret_cast < ResourceManager *>(ptr);

    HttpClient *request = new HttpClient();
    std::string uri = "http://s.web2.qq.com/api/get_group_info?gcode=%5B"+
                      gcode+"%5D&retainKey=memo%2Cgcode&vfwebqq="+vfwebqq + \
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
          gcode+ "&vfwebqq="+ vfwebqq+"&t=1339476485660";

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


Poll2::Poll2(const std::string & data)
{
    this->body = data;
}

void Poll2::run( void * ptr)
{
    while(1)
    {
        HttpClient * client;
        client = new HttpClient();
        std::list<std::string> headers;

        headers.push_back("Referer: http://d.web2.qq.com/proxy.html");

        client->setHttpHeaders(headers);

        std::string result = client->requestServer("http://d.web2.qq.com/channel/poll2",body);
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
                    res->event_queue.push(std::make_pair<QQEvent, std::string>(ON_BUDDY_MESSAGE, value));
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

                    res->event_queue.push(std::make_pair<QQEvent, std::string>(ON_BUDDY_STATUS_CHANGE, value));
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
                    res->event_queue.push(std::make_pair<QQEvent, std::string>( ON_GROUP_MESSAGE , value));
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
