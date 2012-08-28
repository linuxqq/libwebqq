/**
 * @file   QQTask.h
 * @author Xiang Wang <xiang_wang@trendmicro.com.cn>
 * @date   Wed Jul 25 17:16:11 2012
 *
 * @brief
 *
 *
 */

#ifndef __QQ_TASK_H__
#define __QQ_TASK_H__
#include <string>
#include "ThreadPool.h"

class QQTask{

public:

    QQTask(const std::string &uin, const std::string & vfwebqq );

    ~QQTask();

    std::string uin;
    std::string vfwebqq;

};

class GetLongNick:public ThreadPool::TPool::TJob, public QQTask{
public:
    GetLongNick(const std::string & uin, const std::string & vfwebqq);

    virtual void run(void *);
};

class GetFriendUin:public ThreadPool::TPool::TJob, public QQTask{
public:
    GetFriendUin( const std::string & uin , const std::string & vfwebqq);
    virtual void run(void *);

};


class GetFriendsInfo2: public ThreadPool::TPool::TJob , QQTask
{

public:
    GetFriendsInfo2( const std::string & uin  , const std::string & vfwebqq );
    virtual void run( void *);
};

class SendShake:public ThreadPool::TPool::TJob
{
    std::string to_uin;
    std::string client_id;
    std::string psessionid;
public:
    SendShake(const std::string & uin_,
              const std::string & client_id_,
              const std::string & psessionid_ ):
        to_uin(uin_), client_id(client_id_), psessionid(psessionid_)
    {}
    virtual void run( void *);
};

class GetGroupInfo: public ThreadPool::TPool::TJob, public QQTask
{
    std::string gcode;
public:
    GetGroupInfo(const std::string & gcode, const std::string & vfwebqq);
    virtual void run( void *);
};

class GetFace:public ThreadPool::TPool::TJob, public QQTask
{
    static int host_number;
    int type;
public:
    GetFace(const std::string & uin, const std::string & vfwebqq, int type);
    virtual void run(void*);
};

class GetMiscInfo:public ThreadPool::TPool::TJob
{
    std::string vfwebqq;
public:
    GetMiscInfo(const std::string &);
    virtual void run(void *);
};

class SetLongNick : public ThreadPool::TPool::TJob, public QQTask
{
    std::string lnick;
public :
    SetLongNick(const std::string &uin, const std::string & vfwebqq, const std::string & lnick);
    virtual void run(void *);
};

class ChangeStatus : public ThreadPool::TPool::TJob
{
    std::string status;
    std::string clientid;
    std::string psessionid;

public:

    ChangeStatus(const std::string & status_ ,
                 const std::string & clientid_,
                 const std::string & psessionid_):
        status(status_), clientid(clientid_), psessionid(psessionid_)
    {}

    virtual void run(void *ptr);
};

class Poll2:public ThreadPool::TPool::TJob
{
    std::string body;

public:

    Poll2(const std::string & data);
    virtual void run(void *);
};
#endif
