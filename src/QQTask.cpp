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
#include <utility>
#include <sstream>

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

            Json::Value json_result ;
            json_result["uin"] = uin;
            json_result["nick"] = nick;
#ifdef USE_EVENT_QUEUE
            res->event_queue.push(std::make_pair<QQEvent, std::string>(ON_NICK_CHANGE, writer.write(json_result)));
#endif
            if ( res->event_adapter.is_event_registered ( ON_NICK_CHANGE ))
            {
                res->event_adapter.trigger(ON_NICK_CHANGE ,writer.write(json_result));
            }
            else
            {
#ifndef USE_EVENT_QUEUE
                debug_info( " No on message event adapter loaded. (%s,%d)", __FILE__, __LINE__);
#endif
            }
        }
        else
        {
            debug_error("Get friends info2 failed with error code %d ... (%s,%d)", retcode, __FILE__, __LINE__);
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
    while(1)
    {

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
                res->groups[gcode].name = ginfo["name"].asString();
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
                break;
            }
            else{
                debug_error("Get Group member %s list fail ... (%s,%d)", gcode.c_str() ,  __FILE__, __LINE__ );
                sleep(20);
                debug_info("Retry to get group %s memeber list ... (%s,%d)",gcode.c_str(),  __FILE__, __LINE__);
            }
        }catch(...)
        {
            res->ulock();
            debug_error("Cant not parse json body ... (%s,%d)", \
                        __FILE__, __LINE__);
        }
    }
}

int GetFace::host_number = 1 ;

GetFace::GetFace(const std::string & uin, const std::string & vfwebqq, int type_ = 1):QQTask(uin, vfwebqq)
{
    type = type_;
}

void GetFace::run(void * ptr)
{
    ResourceManager * res  = reinterpret_cast<ResourceManager *> (ptr);
    std::stringstream uri ;

    std::list<std::string> cookies = (Singleton<HttpClient>::getInstance())->dumpCookies();

    uri<< "http://"<< "face"<<host_number<<".qun.qq.com"
       << "/cgi/svr/face/getface?cache=0&type="<<type<<"&fid=0&uin="<<uin<<"&vfwebqq="<<vfwebqq;

    host_number = host_number % 10 +1;
    HttpClient *request = new HttpClient(cookies);
    std::list<std::string> headers;
    headers.clear();
    headers.push_back("Referer: http://web.qq.com/");

    request->setHttpHeaders(headers);

    std::string result = request->requestServer(uri.str());

    delete request;
    res->lock();
    if ( res->contacts.count(uin) != 0 || res->groups.count(uin) !=0 )
    {
        res->contacts[uin].face = result;
#ifdef USE_EVENT_QUEUE
        res->event_queue.push(std::make_pair<QQEvent, std::string>(ON_AVATAR_UPDATE, uin));

#endif
        if ( res->event_adapter.is_event_registered (ON_AVATAR_UPDATE))
        {
            res->event_adapter.trigger( ON_AVATAR_UPDATE,uin);
        }
        else
        {
#ifndef USE_EVENT_QUEUE
            debug_info( " No on message event adapter loaded. (%s,%d)", __FILE__, __LINE__);
#endif
        }
    }
    else
    {
        debug_error("Invaid contact uin ... (%s,%d)", __FILE__, __LINE__);
    }
    res->ulock();

}

void SendShake::run(void *ptr)
{
    bool *success = reinterpret_cast<bool *>(ptr);
    std::stringstream uri;
    uri<<"http://d.web2.qq.com/channel/shake2?to_uin="
       <<to_uin<<"&clientid="
       <<client_id<<"&psessionid="<<psessionid
       <<"&t="<<QQUtil::currentTimeMillis();

    HttpClient request;
    std::list<std::string> headers;
    headers.clear();
    headers.push_back("Referer: http://d.web2.qq.com/proxy.html?v=20110331002&callback=1&id=2");
    request.setHttpHeaders(headers);
    std::string result = request.requestServer(uri.str());

    Json::Reader jsonReader;
    Json::Value root;
    jsonReader.parse(result, root, false);
    int ret= root["retcode"].asInt();

    if ( ret == 0 )
    {
        *success = true;
        debug_info("Success to send shake request ... (%s,%d)", __FILE__, __LINE__);
    }
    else
    {
        *success = false;
        debug_error("Fail to send shake request ... (%s,%d)", __FILE__, __LINE__);
    }
}

GetMiscInfo::GetMiscInfo(const std::string &vfwebqq)
{
    this->vfwebqq = vfwebqq;
}

void GetMiscInfo::run(void * ptr)
{
    ResourceManager * res  = reinterpret_cast<ResourceManager *> (ptr);

    int count =0;
    for ( std::map<std::string , QQBuddy>::iterator it = res->contacts.begin();
          it != res->contacts.end(); it ++, count ++ )
    {
        GetFriendsInfo2 * get_info = new GetFriendsInfo2( it->first, vfwebqq);
        GetFace * get_face = new GetFace (it->first, vfwebqq);
        ThreadPool::run(get_info, res, true);
        ThreadPool::run(get_face, res, true);
        if ( count == 80)
        {
            count = 0;
            sleep(5);
        }
    }

    for ( std::map<std::string, QQGroup>::iterator it = res->groups.begin();
          it != res->groups.end(); it++)
    {
        GetFace *get_group_face = new GetFace(it->first, vfwebqq, 4);
        ThreadPool::run(get_group_face, res, true);
    }

}

SetLongNick::SetLongNick(const std::string & uin ,
                         const std::string & vfwebqq,
                         const std::string & nick):QQTask(uin, vfwebqq)
{
    lnick = nick;
}

void SetLongNick::run(void *ptr)
{
    bool * success = reinterpret_cast <bool *> (ptr);

    HttpClient client;
    std::list<std::string> headers;
    headers.push_back("Referer: http://s.web2.qq.com/proxy.html?v=20110412001&callback=1&id=3");
    client.setHttpHeaders(headers);

    Json::Reader jsonReader;
    Json::FastWriter writer;
    Json::Value root;
    root["nlk"] = lnick;
    root["vfwebqq"] = vfwebqq;


    std::string body = "r=" + QQUtil::urlencode(QQUtil::trim(writer.write(root)));

    std::string result = client.requestServer("http://s.web2.qq.com/api/set_long_nick2",
                                              body);

    jsonReader.parse(result, root, false);
    int ret = root["retcode"].asInt();

    if (ret == 0)
    {
        *success = true;
        debug_info("Success to set self long nick! ... (%s,%d)",
                   __FILE__, __LINE__);
    }
    else
    {
        *success = false;
        debug_error("Fail to set self long nick! ... (%s,%d)",
                    __FILE__, __LINE__);
    }

}

void ChangeStatus::run(void * ptr)
{
    bool *success = reinterpret_cast<bool *>(ptr);
    std::stringstream uri;
    uri<<"http://d.web2.qq.com/channel/change_status2?newstatus="
       <<status<<"&clientid="
       <<clientid<<"&psessionid="<<psessionid
       <<"&t="<<QQUtil::currentTimeMillis();

    HttpClient client;
    std::list<std::string> headers;
    headers.push_back("Referer: http://d.web2.qq.com/proxy.html?v=20110331002&callback=1&id=3");
    client.setHttpHeaders(headers);
    std::string result = client.requestServer(uri.str());

    Json::Reader jsonReader;
    Json::Value root;
    jsonReader.parse(result, root, false);
    int ret = root["retcode"].asInt();
    if ( ret == 0)
    {
        *success = true;
        debug_info("Success to change QQ Status as %s! ... (%s,%d)", status.c_str(),
                   __FILE__, __LINE__);
    }
    else
    {
        *success = false;
        debug_error("Fail to change QQ Status !... (%s,%d)" , __FILE__, __LINE__);
    }

}

GetOffPic::GetOffPic(const std::string & from_uin_, const std::string & file_path_, 
                     const std::string & clientid_, const std::string & pessionid_)
{
    this->from_uin = from_uin_;
    this->file_path = file_path_;
    this->clientid = clientid_;
    this->psessionid = pessionid_;
}

void GetOffPic::run(void * ptr)
{
    ResourceManager * res  = reinterpret_cast<ResourceManager *> (ptr);

    std::list<std::string> cookies = (Singleton<HttpClient>::getInstance())->dumpCookies();
    HttpClient * client = NULL;
    client = new HttpClient(cookies);
    std::list<std::string> headers;
    headers.push_back("Referer: http://web.qq.com/");
    client->setHttpHeaders(headers);
    std::stringstream uri ;
    uri <<"http://d.web2.qq.com/channel/get_offpic2?file_path="
        << QQUtil::urlencode(file_path)
        <<"&f_uin="<<from_uin
        <<"&clientid="<<clientid 
        <<"&psessionid="<< psessionid;

    std::string pic = client->requestServer(uri.str());

    delete client;

#ifdef USE_EVENT_QUEUE
    res->event_queue.push(std::make_pair<QQEvent, std::string>(ON_BUDDY_PIC_MESSAGE, pic));
#endif
    if ( res->event_adapter.is_event_registered(ON_BUDDY_PIC_MESSAGE) )
    {
        res->event_adapter.trigger(ON_BUDDY_PIC_MESSAGE ,pic);
    }

}


Poll2::Poll2(const std::string & data,
             const std::string & clientid_,
             const std::string & psessionid_)
{
    this->body = data;
    this->clientid = clientid_;
    this->psessionid = psessionid_;
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
                    
                    Json::Value data = (*iter)["value"]["content"];
                                        
                    for ( int index = 0 ; index < data.size(); ++ index )
                    {
                        Json::ValueType value_type = data[index].type();
                        
                        if ( value_type == Json::arrayValue )
                        {
                            if ( data[index][0].asString() == "font")
                            {
                                continue;
                            }
                            else if ( data[index][0].asString() == "offpic")
                            {
                                std::string from_uin = QQUtil::trim(writer.write((*iter)["value"]["from_uin"]));
                                std::string file_path = data[index][1]["file_path"].asString();
                                GetOffPic *off_pic =  new GetOffPic(from_uin, file_path, clientid, psessionid);
                                ThreadPool::run(off_pic, ptr, true);
                            }
                        }
                        else if (value_type == Json::stringValue )
                        {
#ifdef USE_EVENT_QUEUE
                            res->event_queue.push(std::make_pair<QQEvent, std::string>(ON_BUDDY_MESSAGE,data[index].asString()));
#endif
                            if ( res->event_adapter.is_event_registered(ON_BUDDY_MESSAGE) )
                            {
                                res->event_adapter.trigger(ON_BUDDY_MESSAGE ,data[index].asString());
                            }
                        }
                        else
                        {
                            debug_error("Invalid data format... (%s,%d)", __FILE__, __LINE__);
                        }
                                                
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
#ifndef USE_EVENT_QUEUE
                        debug_info( " No on message event adapter loaded. (%s,%d)", __FILE__, __LINE__);
#endif
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
#ifndef USE_EVENT_QUEUE
                        debug_info( " No on message event adapter loaded. (%s,%d)", __FILE__, __LINE__);
#endif
                    }
                }
                else if (poll_type == "shake_message")
                {
#ifdef USE_EVENT_QUEUE
                    res->event_queue.push(std::make_pair<QQEvent, std::string>( ON_SHAKE_MESSAGE , value));
#endif

                    if ( res->event_adapter.is_event_registered (ON_SHAKE_MESSAGE))
                    {
                        res->event_adapter.trigger(ON_SHAKE_MESSAGE ,value);
                    }
                    else
                    {
#ifndef USE_EVENT_QUEUE
                        debug_info( " No on message event adapter loaded. (%s,%d)", __FILE__, __LINE__);
#endif
                    }
                }
            }
            res->ulock();
        }
    }
}
