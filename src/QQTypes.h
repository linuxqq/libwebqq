/**
 * @file   QQTypes.h
 * @author Xiang Wang <xiang_wang@trendmicro.com.cn>
 * @date   Wed Jun  6 18:41:41 2012
 *
 * @brief
 *
 *
 */

#ifndef INCLUDED_QQTYPES_H
#define INCLUDED_QQTYPES_H
#include <string>
#include <list>
#include "Singleton.h"
#include <boost/function.hpp>

typedef boost::function<void (std::string)> EventListener;

enum  QQEvent{
    ON_BUDDY_MESSAGE = 512,
    ON_GROUP_MESSAGE,
    ON_SEND_MESSAGE,
    ON_RECEIVE_AVATAR,
    ON_BUDDY_STATUS_CHANGE,
    ON_NICK_CHANGE,
    ON_SHAKE_MESSAGE
};

enum QQReturnCode{
    NO_ERROR =0 ,
    PARAMETER_ERR= -1,
    NETWORK_ERR= -2,
    WRONGPWD_ERR= -3,
    WRONGVC_ERR=-4,
    WRONGUIN_ERR= -5,
    OTHER_ERR= -100
};

struct QQFaceImg{
    std::string uin;
    std::string num;
    std::string data;
    std::string type;
};

struct Color{
    int bold, italic, underline;
};

struct QQMsgFont{
    std::string name;
    int size;
    std::string color;
};

enum  QQMsgContentType
{
    QQ_MSG_CONTENT_FACE_T = 256,        // face
    QQ_MSG_CONTENT_STRING_T,            // string
    QQ_MSG_CONTENT_FONT_T,              // font
    QQ_MSG_CONTENT_UNKNOWN_T            // unknown
};

enum QQMsgType{
    MSG_BUDDY_T = 128,      /* buddy message */
    MSG_GROUP_T,            // group message
    MSG_STATUS_CHANGED_T,   // buddy status changed
    MSG_KICK_T,             // kick message. In other place logined
    MSG_UNKNOWN_T
};

struct Birthday {
    int year, month, day;
};

struct QQBuddy{
    std::string  uin;               //the uin. Change every login
    std::string qqnumber;          //the qq number
    std::string status;
    int is_vip;
    int vip_level;
    std::string nick;              //
    std::string markname;

    std::string country;
    std::string province;
    std::string city;

    std::string gender;            //male or female
    std::string face;
    std::string flag;

    Birthday birthday;
    int blood;                 //A, B, AB or O
    int shengxiao;

    int constel;
    std::string phone;
    std::string mobile;
    std::string email;
    std::string occupation;
    std::string college;

    std::string homepage;
    std::string personal;
    std::string lnick;

    int allow;
    int cate_index;            //The index of the category

    /*
      1 : 桌面客户端
      21: 手机客户端
      41: web QQ
    */
    int client_type;

};

struct QQCategory{
    std::string name;
    int index;
};


struct QQGroup{
    std::string name;
    std::string  gid;
    std::string gnumber;
    std::string code;
    std::string flag;
    std::string owner;
    std::string mark;
    std::string mask;
    int option;
    std::string createtime;
    std::string gclass;
    int level;
    std::string face;
    std::string memo;
    std::string fingermemo;
};

struct QQConfig:public Singleton<QQConfig>
{
    friend class Singleton<QQConfig>;
    bool use_proxy;
    std::string proxy_type;
    std::string proxy_host;
    std::string proxy_port;
    std::string proxy_account;
    std::string proxy_passcode;

    QQConfig(){
        use_proxy = false;
    }
private:
    void reset(){}
};

#endif /* INCLUDED_QQTYPES_H */

