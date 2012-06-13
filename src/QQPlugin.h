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

#include<pthread.h>

#include "QQTypes.h"
#include "Action.h"
#include "json/json.h"

struct ResourceManager:Singleton<ResourceManager>
{

    ResourceManager();
    friend class Singleton<ResourceManager>;
    pthread_mutex_t rw_mutex ;

    void reset(){ }
public:
    std::map<int, QQCategory> categories;
    std::map<std::string, QQGroup> groups;
    std::map<std::string, QQBuddy>  contacts;
    std::map<std::string, std::list<QQBuddy> > group_contacts;
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
public:

    virtual  ~QQPlugin();

    bool  webqq_login( const std::string &uin, const std::string & password, const std::string & status="online");

    bool webqq_logout();

private:

    QQPlugin();

    void get_user_friends();
    void parse_user_friends(const Json::Value & value);

    void get_group_name_list();


    void reset(){}
    ResourceManager * res;

};
#endif
