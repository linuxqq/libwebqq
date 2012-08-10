/**
 * @file   QQPlugin.h
 * @author Xiang Wang <xiang_wang@trendmicro.com.cn>
 * @date   Wed Jun 13 10:28:26 2012
 *
 * @brief
 *
 *
 */

#ifndef __QQ_PLUGIN_H__
#define __QQ_PLUGIN_H__

#include<Singleton.h>
#include <iostream>
#include <map>
#include <string>
#include <list>
#include <queue>
#include<pthread.h>

#include "QQTypes.h"
#include "QQAction.h"
#include "json/json.h"
#include "ThreadPool.h"
#include "QQEventQueue.h"

struct ResourceManager:Singleton<ResourceManager>
{

    ResourceManager();
    friend class Singleton<ResourceManager>;
    ThreadPool::TMutex rw_mutex;

    void reset(){ }
public:
    std::map<int, QQCategory> categories;
    std::map<std::string, QQGroup> groups;
    std::map<std::string, QQBuddy>  contacts;
    std::map<std::string, std::map<std::string, QQBuddy> > group_contacts;

    QQEventQueue event_queue;

    Adapter event_adapter;

    virtual ~ResourceManager();

    void lock();
    void ulock();
};

class QQPlugin: public Singleton<QQPlugin>
{

    friend class Singleton<QQPlugin>;
    std::string vfwebqq;
    std::string clientid;
    std::string psessionid;
    std::string ptwebqq;

    static int message_id;

public:
    QQPlugin();
    virtual  ~QQPlugin();


    class SendBuddyMessage:public ThreadPool::TPool::TJob{
        std::string body;
    public:
        SendBuddyMessage(const std::string & body);
        virtual void run(void *);
    };

    class SendGroupMessage: public ThreadPool::TPool::TJob
    {
        std::string body;
    public:
        SendGroupMessage( const std::string & body);
        virtual void run( void *);
    };

    bool  webqq_login( const std::string &uin, const std::string & password, const std::string & status="online");

    bool send_buddy_message(const std::string & uin, const std::string & message_body );

    bool send_group_message( const std::string & group_class , const std::string & message_body);

    //bool send_group_message(const std::string & uin, const std::string & message_body);

    //bool webqq_logout();

private:

    void get_user_friends();
    void parse_user_friends(const Json::Value & value);

    void get_group_name_list();

    void get_group_info();

    void get_online_buddies();

    void reset(){}
    ResourceManager * res;

};
#endif
